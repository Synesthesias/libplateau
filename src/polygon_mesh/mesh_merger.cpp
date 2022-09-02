#include "mesh_merger.h"
#include "plateau/polygon_mesh/polygon_mesh_utils.h"
#include "citygml/texture.h"
#include <plateau/geometry/geo_coordinate.h>
#include <plateau/geometry/geo_reference.h>

namespace plateau::polygonMesh {
    using namespace citygml;
    using namespace plateau::geometry;

    namespace {

        bool isValidPolygon(const Polygon& other_poly) {
            return !(other_poly.getVertices().empty() || other_poly.getIndices().empty());
        }

        /// 形状情報をマージします。merge関数における SubMesh を扱わない版です。
        /// 座標変換を伴います。
        void mergeShapeWithConvert(Mesh& mesh, const Polygon& other_poly, const TVec2f& uv_2_element,
                                   const TVec2f& uv_3_element,
                                   const MeshExtractOptions& options) {
            unsigned prev_num_vertices = mesh.getVertices().size();
            // 極座標を受け取ります。
            const auto& vertices_lat_lon = other_poly.getVertices();
            const auto& other_indices = other_poly.getIndices();

            // 極座標から平面直角座標へ変換します。
            // TODO 下のenumキャストをなくす。
            //  TODO GeoReferenceを毎回生成するのは効率が悪い
            const auto geo_reference = GeoReference(options.reference_point, options.unit_scale, (CoordinateSystem)options.mesh_axes);
            auto vertices_xyz = std::vector<TVec3d>();
            vertices_xyz.reserve(vertices_lat_lon.size());
            for(const auto& lat_lon : vertices_lat_lon){
                auto xyz = geo_reference.project(GeoCoordinate(lat_lon.x, lat_lon.y, lat_lon.z));
                vertices_xyz.push_back(xyz);
            }

            mesh.addVerticesList(vertices_xyz, options);
            mesh.addIndicesList(other_indices, prev_num_vertices);
            mesh.addUV1(other_poly);
            mesh.addUV2WithSameVal(uv_2_element, vertices_xyz.size());
            mesh.addUV3WithSameVal(uv_3_element, vertices_xyz.size());
        }

        /**
         * merge関数 のテクスチャあり版です。
         * テクスチャについては、マージした結果、範囲とテクスチャを対応付ける SubMesh が追加されます。
         */
        void mergeWithTexture(Mesh& mesh, const Polygon& other_poly, const MeshExtractOptions& options,
                              const TVec2f& uv_2_element, const TVec2f& uv_3_element) {
            if (!isValidPolygon(other_poly)) return;
            mergeShapeWithConvert(mesh, other_poly, uv_2_element, uv_3_element, options);

            const auto& texture = other_poly.getTextureFor("rgbTexture");
            std::string texture_path;
            if (texture == nullptr) {
                texture_path = std::string("");
            } else {
                texture_path = texture->getUrl();
            }
            size_t indices_size = other_poly.getIndices().size();
            mesh.addSubMesh(texture_path, indices_size);
        }

        /**
         * merge関数 のテクスチャ無し版です。
         * 生成される Mesh の SubMesh はただ1つであり、そのテクスチャパスは空文字列となります。
         */
        void mergeWithoutTexture(Mesh& mesh, const Polygon& other_poly, const TVec2f& uv_2_element,
                                 const TVec2f& uv_3_element, const MeshExtractOptions& options) {
            if (!isValidPolygon(other_poly)) return;
            mergeShapeWithConvert(mesh, other_poly, uv_2_element, uv_3_element, options);
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

    void MeshMerger::merge(Mesh& mesh, const Polygon& other_poly, MeshExtractOptions options,
                           const TVec2f& uv_2_element,
                           const TVec2f& uv_3_element) {
        if (!isValidPolygon(other_poly)) return;
        if (options.export_appearance) {
            mergeWithTexture(mesh, other_poly, options, uv_2_element, uv_3_element);
        } else {
            mergeWithoutTexture(mesh, other_poly, uv_2_element, uv_3_element, options);
        }
    }

    void MeshMerger::mergePolygonsInCityObject(Mesh& mesh, const CityObject& city_object, unsigned int lod,
                                   const MeshExtractOptions& options, const TVec2f& uv_2_element,
                                   const TVec2f& uv_3_element) {
        auto polygons = findAllPolygons(city_object, lod);
        for (auto poly: polygons) {
            MeshMerger::merge(mesh, *poly, options, uv_2_element, uv_3_element);
        }
    }

    void MeshMerger::mergePolygonsInCityObjects(Mesh& mesh, const std::list<const CityObject*>& city_objects,
                                    unsigned int lod,
                                    const TVec2f& uv_3_element, const MeshExtractOptions& options,
                                    const TVec2f& uv_2_element) {
        for (auto obj: city_objects) {
            mergePolygonsInCityObject(mesh, *obj, lod, options, uv_2_element, uv_3_element);
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
