#include "mesh_merger.h"
#include "citygml/texture.h"
#include <plateau/geometry/geo_coordinate.h>
#include <plateau/geometry/geo_reference.h>

#include <filesystem>

namespace plateau::polygonMesh {
    using namespace citygml;
    using namespace plateau::geometry;

    namespace {

        bool isValidPolygon(const Polygon& other_poly) {
            return !(other_poly.getVertices().empty() || other_poly.getIndices().empty());
        }

        /// 形状情報をマージします。merge関数における SubMesh を扱わない版です。
        /// 座標変換を伴います。
        void mergeShapeWithConvertCoords(Mesh& mesh, const Polygon& other_poly, const TVec2f& uv_2_element,
                                         const TVec2f& uv_3_element, const GeoReference& geo_reference,
                                         bool invert_mesh_front_back) {
            unsigned prev_num_vertices = mesh.getVertices().size();
            // 極座標を受け取ります。
            const auto& vertices_lat_lon = other_poly.getVertices();
            const auto& other_indices = other_poly.getIndices();

            // 極座標から平面直角座標へ変換します。
            auto vertices_xyz = std::vector<TVec3d>();
            vertices_xyz.reserve(vertices_lat_lon.size());
            for(const auto& lat_lon : vertices_lat_lon){
                auto xyz = geo_reference.project(lat_lon);
                vertices_xyz.push_back(xyz);
            }

            mesh.addVerticesList(vertices_xyz);
            mesh.addIndicesList(other_indices, prev_num_vertices, invert_mesh_front_back);
            mesh.addUV1(other_poly);
            mesh.addUV2WithSameVal(uv_2_element, vertices_xyz.size());
            mesh.addUV3WithSameVal(uv_3_element, vertices_xyz.size());
        }

        /**
         * merge関数 のテクスチャあり版です。
         * テクスチャについては、マージした結果、範囲とテクスチャを対応付ける SubMesh が追加されます。
         */
        void
        mergeWithTexture(Mesh& mesh, const Polygon& other_poly, const TVec2f& uv_2_element, const TVec2f& uv_3_element,
                         const GeoReference& geo_reference, bool invert_mesh_front_back, const std::string& gml_path) {
            if (!isValidPolygon(other_poly)) return;
            mergeShapeWithConvertCoords(mesh, other_poly, uv_2_element, uv_3_element, geo_reference, invert_mesh_front_back);

            const auto& texture = other_poly.getTextureFor("rgbTexture");
            std::string texture_path;
            if (texture == nullptr) {
                texture_path = std::string("");
            } else {
                // テクスチャパスを相対から絶対に変換
                texture_path = texture->getUrl();
                std::filesystem::path tpath = std::filesystem::u8path(texture_path);
                if (tpath.is_relative()) {
                    std::filesystem::path a_path = gml_path;
                    a_path = a_path.parent_path();
                    a_path /= tpath;
                    auto abs_path = std::filesystem::absolute(a_path);

                    texture_path = abs_path.u8string();
                }
            }
            size_t indices_size = other_poly.getIndices().size();
            mesh.addSubMesh(texture_path, indices_size);
        }

        /**
         * merge関数 のテクスチャ無し版です。
         * 生成される Mesh の SubMesh はただ1つであり、そのテクスチャパスは空文字列となります。
         */
        void mergeWithoutTexture(Mesh& mesh, const Polygon& other_poly, const TVec2f& uv_2_element,
                                 const TVec2f& uv_3_element,
                                 const GeoReference& geo_reference, bool invert_mesh_front_back) {
            if (!isValidPolygon(other_poly)) return;
            mergeShapeWithConvertCoords(mesh, other_poly, uv_2_element, uv_3_element, geo_reference, invert_mesh_front_back);
            mesh.extendLastSubMesh();

        }

        /**
         * findAllPolygons のジオメトリを対象とする版です。
         * 結果は引数の polygons に格納します。
         */
        void findAllPolygonsInGeometry(const Geometry& geom, std::list<const citygml::Polygon*>& polygons, unsigned lod) {
            unsigned int num_child = geom.getGeometriesCount();
            for (unsigned int i = 0; i < num_child; i++) {
                findAllPolygonsInGeometry(geom.getGeometry(i), polygons, lod);
            }

            if (geom.getLOD() != lod) return;

            unsigned int num_poly = geom.getPolygonsCount();
            for (unsigned int i = 0; i < num_poly; i++) {
                polygons.push_back(geom.getPolygon(i).get());
            }
        }

    }

    namespace{ // merge で使う無名名前空間関数です。
        /**
         * PLATEAUからメッシュを読み込んで座標軸を変換をするとき、このままだとメッシュが裏返ることがあります（座標軸が反転したりするので）。
         * 裏返りを補正する必要があるかどうかを bool で返します。
         */
        bool shouldInvertIndicesOnMeshConvert(CoordinateSystem sys){
            switch(sys){
                case CoordinateSystem::ENU:
                case CoordinateSystem::WUN:
                case CoordinateSystem::NWU: return false;
                case CoordinateSystem::EUN: return true;
                default: throw std::runtime_error("Unknown coordinate system.");
            }
        }
    } // merge で使う無名名前空間関数

    void MeshMerger::merge(Mesh& mesh, const Polygon& other_poly, const MeshExtractOptions& mesh_extract_options,
                           const GeoReference& geo_reference, const TVec2f& uv_2_element,
                           const TVec2f& uv_3_element, const std::string& gml_path) {
        if (!isValidPolygon(other_poly)) return;
        bool invert_mesh_front_back = shouldInvertIndicesOnMeshConvert(mesh_extract_options.mesh_axes);
        if (mesh_extract_options.export_appearance) {
            mergeWithTexture(mesh, other_poly, uv_2_element, uv_3_element, geo_reference, invert_mesh_front_back, gml_path);
        } else {
            mergeWithoutTexture(mesh, other_poly, uv_2_element, uv_3_element, geo_reference, invert_mesh_front_back);
        }
    }

    void MeshMerger::mergePolygonsInCityObject(Mesh& mesh, const CityObject& city_object, unsigned int lod,
                                               const MeshExtractOptions& mesh_extract_options, const GeoReference& geo_reference,
                                               const TVec2f& uv_2_element, const TVec2f& uv_3_element, const std::string& gml_path) {
        auto polygons = findAllPolygons(city_object, lod);
        for (auto poly: polygons) {
            MeshMerger::merge(mesh, *poly, mesh_extract_options, geo_reference, uv_2_element, uv_3_element, gml_path);
        }
    }

    void MeshMerger::mergePolygonsInCityObjects(Mesh& mesh, const std::list<const CityObject*>& city_objects,
                                                unsigned int lod,
                                                const MeshExtractOptions& mesh_extract_options, const GeoReference& geo_reference,
                                                const TVec2f& uv_3_element, const TVec2f& uv_2_element, const std::string& gml_path) {
        for (auto obj: city_objects) {
            mergePolygonsInCityObject(mesh, *obj, lod, mesh_extract_options, geo_reference, uv_2_element, uv_3_element, gml_path);
        }
    }

    std::list<const Polygon*> MeshMerger::findAllPolygons(const CityObject& city_obj, unsigned lod) {
        auto polygons = std::list<const citygml::Polygon*>();
        unsigned int num_geom = city_obj.getGeometriesCount();
        for (unsigned int i = 0; i < num_geom; i++) {
            findAllPolygonsInGeometry(city_obj.getGeometry(i), polygons, lod);
        }
        return std::move(polygons);
    }
}
