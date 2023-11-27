#include <plateau/polygon_mesh/mesh_factory.h>
#include "citygml/texture.h"
#include "citygml/material.h"
#include <plateau/geometry/geo_coordinate.h>
#include <plateau/geometry/geo_reference.h>
#include <cassert>

#include <filesystem>

#include "plateau/polygon_mesh/mesh_merger.h"



namespace plateau::polygonMesh {
    using namespace citygml;
    using namespace plateau::geometry;

    namespace {

        bool isValidPolygon(const Polygon& other_poly) {
            return !(other_poly.getVertices().empty() || other_poly.getIndices().empty());
        }

        /**
         * Plateau の Polygon を Mesh情報 に変換します。
         * Mesh構築に必要な情報を Polygon から コピーします。すなわち:
         * Vertices を極座標から平面直角座標に変換したうえでコピーします。座標軸は 入力も出力も ENU です。
         * Indices, UV1 をコピーします。SubMeshを生成します。
         * 引数の gml_path は、テクスチャパスを相対から絶対に変換するときの基準パスです。
         * 結果は引数で out と名の付くものに格納されます。
         */
        void cityGmlPolygonToMesh(
            const Polygon& polygon, const std::string& gml_path,
            const GeoReference& geo_reference, Mesh& out_mesh) {

            // マージ対象の情報を取得します。ここでの頂点は極座標です。
            const auto& vertices_lat_lon = polygon.getVertices();
            const auto& in_indices = polygon.getIndices();
            auto in_uv_1 = polygon.getTexCoordsForTheme("rgbTexture", true);
            // rgbTextureのthemeが存在しない場合
            if (in_uv_1.empty()) {
                auto themes = polygon.getAllTextureThemes(true);
                if (!themes.empty())
                    in_uv_1 = polygon.getTexCoordsForTheme(themes.at(0), true);
            }

            assert(in_indices.size() % 3 == 0);

            if (vertices_lat_lon.empty() || in_indices.empty())
                return;

            // 極座標から平面直角座標へ変換します。
            auto& out_vertices = out_mesh.getVertices();
            out_vertices.reserve(vertices_lat_lon.size());
            for (const auto& lat_lon : vertices_lat_lon) {
                auto xyz = geo_reference.projectWithoutAxisConvert(lat_lon);
                out_vertices.push_back(xyz);
            }
            assert(out_vertices.size() == vertices_lat_lon.size());

            // Indicesをコピーします。
            out_mesh.addIndicesList(in_indices, false, false);

            assert(out_mesh.getIndices().size() == in_indices.size());
            assert(out_mesh.getIndices().size() % 3 == 0);

            // UV1をコピーし、頂点数に足りない分を 0 で埋めます。
            out_mesh.addUV1(in_uv_1, vertices_lat_lon.size());

            // テクスチャパスを取得し SubMesh を作ります。
            auto texture = polygon.getTextureFor("rgbTexture");
            if (texture == nullptr) {
                // rgbTextureのthemeが存在しない場合
                auto themes = polygon.getAllTextureThemes(true);
                if (!themes.empty())
                    texture = polygon.getTextureFor(themes.at(0));
            }
            std::string texture_path;
            if (texture == nullptr) {
                texture_path = std::string("");
            } else {
                // テクスチャパスを相対から絶対に変換
                texture_path = texture->getUrl();
                const auto relative_texture_path = std::filesystem::u8path(texture_path);
                if (relative_texture_path.is_relative()) {
                    auto a_path = std::filesystem::u8path(gml_path);
                    a_path = a_path.parent_path();
                    a_path /= relative_texture_path;
                    const auto abs_path = std::filesystem::absolute(a_path);

                    texture_path = abs_path.u8string();
                }
            }

            // Materialを取得し SubMesh に設定します。設定されていない場合nullptrが設定されます。
            auto material = polygon.getMaterialFor("rgbTexture");
            if (material == nullptr) {
                // rgbTextureのthemeが存在しない場合
                auto themes = polygon.getAllMaterialThemes(true);
                if (!themes.empty())
                    material = polygon.getMaterialFor(themes.at(0));
            }

            out_mesh.addSubMesh(texture_path, material, 0, out_mesh.getIndices().size() - 1, -1);
        }

        void findAllPolygonsInGeometry(
            const Geometry& geom, std::list<const citygml::Polygon*>& polygons,
            const unsigned lod, long long& out_vertices_count) {

            // 子のジオメトリのポリゴンをすべて取得
            const unsigned int child_count = geom.getGeometriesCount();
            for (unsigned int i = 0; i < child_count; i++) {
                findAllPolygonsInGeometry(geom.getGeometry(i), polygons, lod, out_vertices_count);
            }

            if (geom.getLOD() != lod) return;

            const unsigned int polygon_count = geom.getPolygonsCount();
            for (unsigned int i = 0; i < polygon_count; i++) {
                const auto& poly = geom.getPolygon(i);
                polygons.push_back(poly.get());
                out_vertices_count += static_cast<long long>(poly->getVertices().size());
            }
        }

        void findAllPolygonsInGeometry(
            const Geometry& geom, std::list<const citygml::Polygon*>& polygons,
            const unsigned lod, long long& out_vertices_count,
            const std::vector<plateau::geometry::Extent> extents) {

            // 子のジオメトリのポリゴンをすべて取得
            const unsigned int child_count = geom.getGeometriesCount();
            for (unsigned int i = 0; i < child_count; i++) {
                findAllPolygonsInGeometry(geom.getGeometry(i), polygons, lod, out_vertices_count, extents);
            }

            if (geom.getLOD() != lod) return;

            // 1つでも頂点が範囲内にあるかどうかを探索
            const unsigned int polygon_count = geom.getPolygonsCount();
            for (unsigned int i = 0; i < polygon_count; i++) {
                const auto& polygon = geom.getPolygon(i);
                bool is_in_extent = false;
                // TODO: 計算コストが頂点数と範囲数に比例するため高速化
                for (const auto& vertex : polygon->getVertices()) {
                    for (const auto& extent : extents) {
                        if (extent.contains(vertex)) {
                            is_in_extent = true;
                            break;
                        }
                    }
                    if (is_in_extent)
                        break;
                }

                if (!is_in_extent)
                    continue;

                polygons.push_back(polygon.get());
                out_vertices_count += static_cast<long long>(polygon->getVertices().size());
            }
        }
    }

    bool MeshFactory::shouldInvertIndicesOnMeshConvert(const CoordinateSystem sys) {
        switch (sys) {
        case CoordinateSystem::ENU:
        case CoordinateSystem::WUN:
            return false;
        case CoordinateSystem::ESU:
        case CoordinateSystem::EUN:
            return true;
        default:
            throw std::runtime_error("Unknown coordinate system.");
        }
    }

    MeshFactory::MeshFactory(
        std::unique_ptr<Mesh>&& target,
        const MeshExtractOptions& mesh_extract_options,
        const std::vector<plateau::geometry::Extent>& extents,
        const geometry::GeoReference& geo_reference)

        : options_(mesh_extract_options)
        , geo_reference_(geo_reference)
        , extents_(extents) {

        if (target == nullptr)
            mesh_ = std::make_unique<Mesh>();
        else
            mesh_ = std::move(target);
    }

    void MeshFactory::addPolygon(const Polygon& polygon, const std::string& gml_path) const {
        if (!isValidPolygon(polygon))
            return;

        Mesh mesh;
        cityGmlPolygonToMesh(polygon, gml_path, geo_reference_, mesh);

        const auto from_axis = geometry::CoordinateSystem::ENU;
        const auto to_axis = options_.mesh_axes;

        // 座標軸を変換します。
        auto& vertices = mesh.getVertices();
        const auto vertex_count = vertices.size();
        for (size_t i = 0; i < vertex_count; ++i) {
            const auto& before = vertices.at(i);
            // BEFORE → ENU
            const auto enu_vertex = GeoReference::convertAxisToENU(from_axis, before);
            // ENU → AFTER
            vertices.at(i) = GeoReference::convertAxisFromENUTo(to_axis, enu_vertex);
        }

        // 座標軸を変換するとき、符号の反転によってポリゴンが裏返ることがあります。それを補正するためにポリゴンを裏返す処理が必要かどうかを求めます。
        // 座標軸を FROM から TO に変換するとして、それは 下記の [1]と[2] の XOR で求まります。
        const bool invert_mesh_front_back =
            shouldInvertIndicesOnMeshConvert(from_axis) !=  // [1] FROM → ENU に変換するときに反転の必要があるか
            shouldInvertIndicesOnMeshConvert(to_axis);     // [2] ENU → TO に変換するときに反転の必要があるか

        MeshMerger::mergeMesh(
            *mesh_,
            mesh,
            invert_mesh_front_back,
            options_.export_appearance
        );
    }

    void MeshFactory::addPolygonsInPrimaryCityObject(
        const CityObject& city_object, const unsigned int lod,
        const std::string& gml_path) {

        long long vertex_count = 0;
        std::list<const Polygon*> polygons;

        findAllPolygons(city_object, lod, polygons, vertex_count);
        mesh_->reserve(vertex_count);
        for (const auto polygon : polygons) {
            addPolygon(*polygon, gml_path);
        }

        const auto& gml_id = city_object.getId();

        const auto primary_index = available_primary_index_.getPrimary();
        mesh_->addUV4WithSameVal(primary_index.toUV(), vertex_count);
        mesh_->city_object_list_.add(primary_index, gml_id);
    }

    long long MeshFactory::countVertices(
        const CityObject& city_object, const unsigned lod) {

        long long vertex_count = 0;
        std::list<const Polygon*> polygons;
        findAllPolygons(city_object, lod, polygons, vertex_count);

        return vertex_count;
    }

    void MeshFactory::addPolygonsInAtomicCityObject(
        const CityObject& parent_city_object, const CityObject& city_object,
        const unsigned lod, const std::string& gml_path) {

        CityObjectIndex city_object_index{};

        if (last_parent_gml_id_cache_ == parent_city_object.getId()) {
            city_object_index = last_atomic_index_cache_;
            ++city_object_index.atomic_index;
        } else {
            city_object_index = createAvailableAtomicIndex(parent_city_object.getId());
            // 新規の主要地物に最小地物を追加する際は、主要地物のインデックスも追加する。
            if (city_object_index.atomic_index == 0) {
                auto primary_index = city_object_index;
                primary_index.atomic_index = CityObjectIndex::invalidIndex();
                mesh_->city_object_list_.add(primary_index, parent_city_object.getId());
            }
        }

        last_atomic_index_cache_ = city_object_index;
        last_parent_gml_id_cache_ = parent_city_object.getId();

        long long vertex_count = 0;
        std::list<const Polygon*> polygons;
        findAllPolygons(city_object, lod, polygons, vertex_count);
        mesh_->reserve(vertex_count);
        for (const auto polygon : polygons) {
            addPolygon(*polygon, gml_path);
        }

        mesh_->addUV4WithSameVal(city_object_index.toUV(), vertex_count);
        mesh_->city_object_list_.add(city_object_index, city_object.getId());
    }

    void MeshFactory::addPolygonsInAtomicCityObjects(
        const CityObject& parent_city_object,
        const std::list<const CityObject*>& city_objects,
        const unsigned lod, const std::string& gml_path) {

        auto available_city_object_index = createAvailableAtomicIndex(parent_city_object.getId());
        // 新規の主要地物に最小地物を追加する際は、主要地物のインデックスも追加する。
        if (available_city_object_index.atomic_index == 0) {
            auto primary_index = available_city_object_index;
            primary_index.atomic_index = CityObjectIndex::invalidIndex();
            mesh_->city_object_list_.add(primary_index, parent_city_object.getId());
        }

        for (const auto city_object : city_objects) {
            auto gml_id = city_object->getId();

            long long vertex_count = 0;
            std::list<const Polygon*> polygons;
            findAllPolygons(*city_object, lod, polygons, vertex_count);
            mesh_->reserve(vertex_count);
            for (const auto polygon : polygons) {
                addPolygon(*polygon, gml_path);
            }

            if (vertex_count > 0)
                mesh_->addUV4WithSameVal(available_city_object_index.toUV(), vertex_count);

            mesh_->city_object_list_.add(available_city_object_index, gml_id);
            ++available_city_object_index.atomic_index;
        }
    }

    void MeshFactory::incrementPrimaryIndex() {
        ++available_primary_index_.primary_index;
    }

    void MeshFactory::findAllPolygons(
        const CityObject& city_obj, const unsigned lod,
        std::list<const Polygon*>& out_polygons, long long& out_vertices_count) {

        const auto geometry_count = city_obj.getGeometriesCount();
        out_vertices_count = 0;

        std::vector<Extent> extents = { Extent::all() };
        if (options_.exclude_polygons_outside_extent)
            extents = extents_;

        for (unsigned i = 0; i < geometry_count; i++) {
            findAllPolygonsInGeometry(
                city_obj.getGeometry(i), out_polygons, lod,
                out_vertices_count, extents);
        }
    }

    CityObjectIndex MeshFactory::createAvailableAtomicIndex(const std::string& parent_gml_id) {
        const auto it = available_atomic_indices_.find(parent_gml_id);
        CityObjectIndex available_city_object_index{};
        if (it != available_atomic_indices_.end()) {
            // 既に主要地物のインデックスが登録されていた場合はそれを利用
            available_city_object_index = it->second;
        } else {
            // 登録されて無ければ新規にインデックスを追加
            available_city_object_index = available_primary_index_;
            available_city_object_index.atomic_index = 0;
        }
        return available_city_object_index;
    }
}
