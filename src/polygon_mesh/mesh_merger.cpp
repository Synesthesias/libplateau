#include <plateau/polygon_mesh/mesh_merger.h>
#include "citygml/texture.h"
#include <plateau/geometry/geo_coordinate.h>
#include <plateau/geometry/geo_reference.h>
#include <cassert>

#include <filesystem>

namespace plateau::polygonMesh {
    using namespace citygml;
    using namespace plateau::geometry;

    namespace {

        bool isValidPolygon(const Polygon& other_poly) {
            return !(other_poly.getVertices().empty() || other_poly.getIndices().empty());
        }

        bool isValidMesh(const Mesh& mesh) {
            return !(mesh.getVertices().empty() || mesh.getIndices().empty());
        }

        /// 3Dメッシュのうち範囲外の部分を除去します。
        /// 具体的には、頂点、インデックス、UVのリストと範囲を受け取り、そのうち範囲外の頂点とポリゴンを除いたものを outと名前の付いた引数のvectorに追加します。
        /// 頂点と範囲は極座標（緯度、経度、高さ）であることが前提です。
        void removeTrianglesOutsideExtent(Extent extent,
                                          const std::vector<TVec3d>& src_vertices,
                                          const std::vector<unsigned int>& src_indices,
                                          const std::vector<TVec2f>& src_uv_1,
                                          std::vector<TVec3d>& out_vertices, std::vector<unsigned int>& out_indices,
                                          std::vector<TVec2f>& out_uv_1) {
            auto src_vertices_count = src_vertices.size();
            auto src_indices_count = src_indices.size();
            out_vertices.reserve(src_vertices_count + out_vertices.size());
            out_indices.reserve(src_indices.size() + out_indices.size());
            out_uv_1.reserve(src_uv_1.size() + out_uv_1.size());
            // src_vertices から範囲外のものを除去してインデックスを詰めたとき、src_vertices のi番目 が new_id 番目に移動します。
            // ここで、 i と new_id の対応表をつくります。
            // すなわち、 next_vertex_ids を定義し、 その i番目の値が new_id である vector を作ります。
            // ただし i番目の頂点が除去されている場合は new_id の代わりに -1 が入ります。
            auto next_vertex_ids = std::vector<long long>();
            next_vertex_ids.reserve(src_vertices.size());
            unsigned long long removed_count = 0;
            for (auto i = 0; i < src_vertices_count; i++) {
                bool in_range = extent.contains(src_vertices.at(i));
                if (in_range) {
                    next_vertex_ids.push_back((long long)(i)-removed_count);
                    assert(i >= removed_count);
                } else {
                    next_vertex_ids.push_back(-1);
                    removed_count++;
                }
            }
            assert(next_vertex_ids.size() == src_vertices_count);

            // src_vertices と src_uv_1 のうち、範囲内のものだけをコピーします。
            for (auto i = 0; i < src_vertices_count; i++) {
                assert(next_vertex_ids.at(i) < (long long)src_vertices_count);
                bool in_range = (next_vertex_ids.at(i) >= 0);
                if (in_range) {
                    out_vertices.push_back(src_vertices.at(i));
                    // UVは必ずしも設定されない（頂点数に対して数が足りない）場合があるため、足りない場合は(0,0)で埋めます。
                    TVec2f uv = (i < src_uv_1.size()) ? src_uv_1.at(i) : TVec2f(0, 0);
                    out_uv_1.push_back(uv);
                }
            }
            assert(out_vertices.size() == src_vertices_count - removed_count);
            assert(out_uv_1.size() == src_vertices_count - removed_count);

            // src_indices のうち、範囲内のものだけをコピーします。
            // idは 3つで1つのポリゴン(三角形) なので、3つ単位で処理します。
            assert(src_indices_count % 3 == 0);
            for (auto i = 0; i < src_indices_count / 3; i++) {
                assert(i * 3 + 2 < src_indices_count);
                auto id_0 = src_indices.at(i * 3);
                auto id_1 = src_indices.at(i * 3 + 1);
                auto id_2 = src_indices.at(i * 3 + 2);
                auto next_id_0 = next_vertex_ids.at(id_0);
                auto next_id_1 = next_vertex_ids.at(id_1);
                auto next_id_2 = next_vertex_ids.at(id_2);
                if (next_id_0 < 0 || next_id_1 < 0 || next_id_2 < 0) { // 頂点が1つでも範囲外のとき
                    continue;
                }
                out_indices.push_back(next_id_0);
                out_indices.push_back(next_id_1);
                out_indices.push_back(next_id_2);
            }
            assert(out_indices.size() % 3 == 0);
        }

        /**
         * Plateau の Polygon を Mesh情報 に変換します。
         * Mesh構築に必要な情報を Polygon から コピーします。すなわち:
         * Vertices を極座標から平面直角座標に変換したうえでコピーします。座標軸は 入力も出力も ENU です。
         * Indices, UV1 をコピーします。SubMeshを生成します。
         * 引数の gml_path は、テクスチャパスを相対から絶対に変換するときの基準パスです。
         * 結果は引数で out と名の付くものに格納されます。
         */
        void plateauPolygonToMeshInfo(
                const Polygon& poly, bool remove_triangles_outside_extent, const std::string& gml_path,
                Extent extent, const GeoReference& geo_reference,
                std::vector<TVec3d>& out_vertices, std::vector<unsigned>& out_indices, std::vector<TVec2f>& out_uv_1,
                std::vector<SubMesh>& out_sub_meshes) {

            // マージ対象の情報を取得します。ここでの頂点は極座標です。
            const auto& vertices_lat_lon_original = poly.getVertices();
            const auto& other_indices_original = poly.getIndices();
            auto other_uv_1_original_base = poly.getTexCoordsForTheme("rgbTexture", true);
            // rgbTextureのthemeが存在しない場合
            if (other_uv_1_original_base.empty()) {
                auto themes = poly.getAllTextureThemes(true);
                if (!themes.empty())
                    other_uv_1_original_base = poly.getTexCoordsForTheme(themes.at(0), true);
            }
            const auto& other_uv_1_original = other_uv_1_original_base;

            assert(other_indices_original.size() % 3 == 0);

            // 範囲外の頂点とポリゴンを除去します（除去する設定の場合）。
            std::vector<TVec3d> out_filtered_vertices;
            std::vector<unsigned int> out_filtered_indices;
            std::vector<TVec2f> out_filtered_uv_1;

            const std::vector<TVec3d>* vertices_lat_lon = &vertices_lat_lon_original;
            const std::vector<unsigned int>* other_indices = &other_indices_original;
            const std::vector<TVec2f>* other_uv_1 = &other_uv_1_original;

            out_filtered_vertices = std::vector<TVec3d>();
            out_filtered_indices = std::vector<unsigned int>();
            out_filtered_uv_1 = std::vector<TVec2f>();
            if (remove_triangles_outside_extent) {
                removeTrianglesOutsideExtent(extent, vertices_lat_lon_original, other_indices_original,
                                             other_uv_1_original,
                                             out_filtered_vertices, out_filtered_indices, out_filtered_uv_1);
                vertices_lat_lon = &out_filtered_vertices;
                other_indices = &out_filtered_indices;
                other_uv_1 = &out_filtered_uv_1;
            }

            assert(other_indices->size() % 3 == 0);

            if (vertices_lat_lon->empty() || other_indices->empty()) return;

            // 極座標から平面直角座標へ変換します。
            out_vertices = std::vector<TVec3d>();
            out_vertices.reserve(vertices_lat_lon->size());
            for (const auto& lat_lon : *vertices_lat_lon) {
                auto xyz = geo_reference.projectWithoutAxisConvert(lat_lon);
                out_vertices.push_back(xyz);
            }
            assert(out_vertices.size() == vertices_lat_lon->size());

            // Indicesをコピーします。
            out_indices = *other_indices;

            assert(out_indices.size() == other_indices->size());
            assert(out_indices.size() % 3 == 0);

            // UV1をコピーし、頂点数に足りない分を 0 で埋めます。
            out_uv_1 = *other_uv_1;
            auto vert_count = out_vertices.size();
            for (auto i = out_uv_1.size(); i < vert_count; i++) {
                out_uv_1.emplace_back(0, 0);
            }

            // テクスチャパスを取得し SubMesh を作ります。
            auto texture = poly.getTextureFor("rgbTexture");
            if (texture == nullptr) {
                // rgbTextureのthemeが存在しない場合
                auto themes = poly.getAllTextureThemes(true);
                if (!themes.empty())
                    texture = poly.getTextureFor(themes.at(0));
            }
            std::string texture_path;
            if (texture == nullptr) {
                texture_path = std::string("");
            } else {
                // テクスチャパスを相対から絶対に変換
                texture_path = texture->getUrl();
                auto tpath = std::filesystem::u8path(texture_path);
                if (tpath.is_relative()) {
                    auto a_path = std::filesystem::u8path(gml_path);
                    a_path = a_path.parent_path();
                    a_path /= tpath;
                    auto abs_path = std::filesystem::absolute(a_path);

                    texture_path = abs_path.u8string();
                }
            }
            out_sub_meshes = std::vector{
                SubMesh(0, out_indices.size() - 1, texture_path)
            };
        }

        /**
         * findAllPolygons のジオメトリを対象とする版です。
         * 結果は引数の polygons に格納します。
         */
        void findAllPolygonsInGeometry(const Geometry& geom, std::list<const citygml::Polygon*>& polygons, unsigned lod, long long& out_vertices_count) {
            unsigned int num_child = geom.getGeometriesCount();
            for (unsigned int i = 0; i < num_child; i++) {
                findAllPolygonsInGeometry(geom.getGeometry(i), polygons, lod, out_vertices_count);
            }

            if (geom.getLOD() != lod) return;

            unsigned int num_poly = geom.getPolygonsCount();
            for (unsigned int i = 0; i < num_poly; i++) {
                const auto& poly = geom.getPolygon(i);
                polygons.push_back(poly.get());
                out_vertices_count += (long long)poly->getVertices().size();
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
        const geometry::GeoReference& geo_reference)

        : options_(mesh_extract_options)
        , geo_reference_(geo_reference)
        , available_primary_index_()
        , last_atomic_index_cache_() {

        if (target == nullptr)
            mesh_ = std::make_unique<Mesh>();
        else
            mesh_ = std::move(target);
    }

    void MeshFactory::addPolygon(const Polygon& polygon, const std::string& gml_path) {
        if (!isValidPolygon(polygon))
            return;

        std::vector<TVec3d> vertices;
        std::vector<unsigned> indices;
        std::vector<TVec2f> uv_1;
        std::vector<SubMesh> sub_meshes;
        plateauPolygonToMeshInfo(
            polygon,
            options_.exclude_triangles_outside_extent,
            gml_path,
            options_.extent,
            geo_reference_,
            vertices,
            indices,
            uv_1,
            sub_meshes
        );

        mergeMeshInfo(
            std::move(vertices),
            std::move(indices), std::move(uv_1), std::move(sub_meshes),
            geometry::CoordinateSystem::ENU,
            options_.mesh_axes,
            options_.export_appearance
        );
    }

    void MeshFactory::mergeMesh(const Mesh& other_mesh, const bool invert_mesh_front_back, const bool include_textures) const {
        if (!isValidMesh(other_mesh))
            return;

        if (include_textures) {
            mergeWithTexture(other_mesh, invert_mesh_front_back);
        } else {
            mergeWithoutTexture(other_mesh, invert_mesh_front_back);
        }
    }

    void MeshFactory::mergeMeshInfo(
        std::vector<TVec3d>&& vertices, std::vector<unsigned>&& indices, UV&& uv_1,
        std::vector<SubMesh>&& sub_meshes,
        CoordinateSystem mesh_axis_convert_from,
        CoordinateSystem mesh_axis_convert_to, bool include_texture) {

        // 座標軸を変換するとき、符号の反転によってポリゴンが裏返ることがあります。それを補正するためにポリゴンを裏返す処理が必要かどうかを求めます。
        // 座標軸を FROM から TO に変換するとして、それは 下記の [1]と[2] の XOR で求まります。
        const bool invert_mesh_front_back =
            shouldInvertIndicesOnMeshConvert(mesh_axis_convert_from) !=  // [1] FROM → ENU に変換するときに反転の必要があるか
            shouldInvertIndicesOnMeshConvert(mesh_axis_convert_to);     // [2] ENU → TO に変換するときに反転の必要があるか

        // 座標軸を変換します。
        std::vector<TVec3d> vs = std::move(vertices);
        const auto vertex_count = vs.size();
        for (int i = 0; i < vertex_count; i++) {
            const auto& before = vs.at(i);
            // BEFORE → ENU
            const auto enu = GeoReference::convertAxisToENU(mesh_axis_convert_from, before);
            // ENU → AFTER
            vs.at(i) = GeoReference::convertAxisFromENUTo(mesh_axis_convert_to, enu);
        }

        const Mesh other_mesh(std::move(vs), std::move(indices), std::move(uv_1), std::move(sub_meshes), CityObjectList());
        mergeMesh(other_mesh, invert_mesh_front_back, include_texture);
    }

    void MeshFactory::addPolygonsInPrimaryCityObject(
        const CityObject& city_object, unsigned int lod,
        const std::string& gml_path) {

        long long vertex_count = 0;
        std::list<const Polygon*> polygons;
        findAllPolygons(city_object, lod, polygons, vertex_count);
        mesh_->reserve(vertex_count);
        for (const auto polygon : polygons) {
            addPolygon(*polygon, gml_path);
        }

        const auto& gml_id = city_object.getId();

        mesh_->addUV4WithSameVal(available_primary_index_.toUV(), vertex_count);
        mesh_->city_object_list_.add(available_primary_index_, gml_id);
        ++available_primary_index_.primary_index;
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
            if (city_object_index.atomic_index == 0)
            {
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

    void MeshFactory::findAllPolygons(const CityObject& city_obj, const unsigned lod, std::list<const Polygon*>& out_polygons, long long& out_vertices_count) {
        const auto geometry_count = city_obj.getGeometriesCount();
        out_vertices_count = 0;
        for (unsigned i = 0; i < geometry_count; i++) {
            findAllPolygonsInGeometry(city_obj.getGeometry(i), out_polygons, lod, out_vertices_count);
        }
    }

    void MeshFactory::mergeWithoutTexture(const Mesh& other_mesh, const bool invert_mesh_front_back) const {
        if (!isValidMesh(other_mesh)) return;
        mergeShape(other_mesh, invert_mesh_front_back);
        mesh_->extendLastSubMesh(mesh_->getIndices().size() - 1);
    }

    void MeshFactory::mergeWithTexture(const Mesh& other_mesh, const bool invert_mesh_front_back) const {
        if (!isValidMesh(other_mesh)) return;
        const auto prev_indices_count = mesh_->getIndices().size();

        mergeShape(other_mesh, invert_mesh_front_back);

        const auto& other_sub_meshes = other_mesh.getSubMeshes();
        const size_t offset = prev_indices_count;
        for (const auto& other_sub_mesh : other_sub_meshes) {
            const auto& texture_path = other_sub_mesh.getTexturePath();
            const size_t start_index = other_sub_mesh.getStartIndex() + offset;
            const size_t end_index = other_sub_mesh.getEndIndex() + offset;
            assert(start_index <= end_index);
            assert(end_index < mesh_->getIndices().size());
            assert((end_index - start_index + 1) % 3 == 0);
            mesh_->addSubMesh(texture_path, start_index, end_index);
        }
    }

    CityObjectIndex MeshFactory::createAvailableAtomicIndex(const std::string& parent_gml_id) {
        const auto it = available_atomic_indices_.find(parent_gml_id);
        CityObjectIndex available_city_object_index;
        if (it != available_atomic_indices_.end()) {
            // 既に主要地物のインデックスが登録されていた場合はそれを利用
            available_city_object_index = it->second;
        } else {
            // 登録されて無ければ新規にインデックスを追加
            available_city_object_index = available_primary_index_;
            available_city_object_index.atomic_index = 0;
            ++available_primary_index_.primary_index;
        }
        return available_city_object_index;
    }
}
