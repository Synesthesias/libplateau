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

        bool isValidMesh(const Mesh& mesh){
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
                    next_vertex_ids.push_back((long long)(i) - removed_count);
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
         * Plateau の Polygon を Mesh に変換します。
         * 新規Meshを作り、そこにPolygonの情報をコピーします。すなわち:
         * Vertices を極座標から平面直角座標に変換したうえでコピーします。
         * Indices, UV1 をコピーします。
         * 引数の gml_path は、テクスチャパスを相対から絶対に変換するときの基準パスです。
         */
        Mesh plateauPolygonToMesh(
                const Polygon& poly, bool remove_triangles_outside_extent, const std::string& gml_path,
                Extent extent, GeoReference geo_reference){
            auto mesh = Mesh(poly.getId());

            // マージ対象の情報を取得します。ここでの頂点は極座標です。
            const auto& vertices_lat_lon_original = poly.getVertices();
            const auto& other_indices_original = poly.getIndices();
            const auto& other_uv_1_original = poly.getTexCoordsForTheme("rgbTexture", true);

            // 範囲外の頂点とポリゴンを除去します（除去する設定の場合）。
            std::vector<TVec3d> out_vertices;
            std::vector<unsigned int> out_indices;
            std::vector<TVec2f> out_uv_1;

            const std::vector<TVec3d>* vertices_lat_lon = &vertices_lat_lon_original;
            const std::vector<unsigned int>* other_indices = &other_indices_original;
            const std::vector<TVec2f>* other_uv_1 = &other_uv_1_original;

            if (remove_triangles_outside_extent) {
                out_vertices = std::vector<TVec3d>();
                out_indices = std::vector<unsigned int>();
                out_uv_1 = std::vector<TVec2f>();
                removeTrianglesOutsideExtent(extent, vertices_lat_lon_original, other_indices_original,
                                             other_uv_1_original,
                                             out_vertices, out_indices, out_uv_1);
                vertices_lat_lon = &out_vertices;
                other_indices = &out_indices;
                other_uv_1 = &out_uv_1;
            }

            // 極座標から平面直角座標へ変換します。
            auto vertices_xyz = std::vector<TVec3d>();
            vertices_xyz.reserve(vertices_lat_lon->size());
            for (const auto& lat_lon: *vertices_lat_lon) {
                auto xyz = geo_reference.project(lat_lon);
                vertices_xyz.push_back(xyz);
            }

            mesh.addVerticesList(vertices_xyz);
            auto vert_count = mesh.getVertices().size();
            // 下の引数 invert_mesh_front_back が false の理由 : ポリゴンの反転は merge 時に行うため、今はしません。
            mesh.addIndicesList(*other_indices, 0, false);
            mesh.addUV1(*other_uv_1, vert_count);
            auto texture = poly.getTextureFor("rgbTexture");
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
            mesh.addSubMesh(texture_path,0, mesh.getIndices().size() - 1 );
            return mesh;
        }



        /// 形状情報をマージします。merge関数における SubMesh を扱わない版です。
        void mergeShape(Mesh& mesh, const Mesh& other_mesh, const TVec2f& uv_2_element, const TVec2f& uv_3_element,
                        bool invert_mesh_front_back) {
            auto prev_num_vertices = mesh.getVertices().size();
            auto other_num_vertices = other_mesh.getVertices().size();

            mesh.addVerticesList(other_mesh.getVertices());
            mesh.addIndicesList(other_mesh.getIndices(), (unsigned) prev_num_vertices, invert_mesh_front_back);
            mesh.addUV1(other_mesh.getUV1(), (unsigned) other_num_vertices);
            mesh.addUV2WithSameVal(uv_2_element, (unsigned) other_num_vertices);
            mesh.addUV3WithSameVal(uv_3_element, (unsigned) other_num_vertices);
        }

        /**
         * merge関数 のテクスチャあり版です。
         * テクスチャについては、マージした結果、範囲とテクスチャを対応付ける SubMesh が追加されます。
         */
        void
        mergeWithTexture(Mesh& mesh, const Mesh& other_mesh, const TVec2f& uv_2_element, const TVec2f& uv_3_element,
                         bool invert_mesh_front_back) {
            if (!isValidMesh(other_mesh)) return;
            auto prev_indices_count = mesh.getIndices().size();

            mergeShape(mesh, other_mesh, uv_2_element, uv_3_element,
                       invert_mesh_front_back);

            const auto& other_sub_meshes = other_mesh.getSubMeshes();
            size_t offset = prev_indices_count;
            for(const auto& other_sub_mesh : other_sub_meshes){
                const auto& texture_path = other_sub_mesh.getTexturePath();
                size_t start_index = other_sub_mesh.getStartIndex() + offset;
                size_t end_index = other_sub_mesh.getEndIndex() + offset;
                assert(start_index <= end_index);
                assert(end_index < mesh.getIndices().size());
                assert((end_index - start_index + 1) % 3 == 0);
                mesh.addSubMesh(texture_path, start_index, end_index);
            }
        }

        /**
         * merge関数 のテクスチャ無し版です。
         * 生成される Mesh の SubMesh はただ1つであり、そのテクスチャパスは空文字列となります。
         */
        void mergeWithoutTexture(Mesh& mesh, const Mesh& other_mesh, const TVec2f& uv_2_element,
                                 const TVec2f& uv_3_element,
                                 bool invert_mesh_front_back) {
            if (!isValidMesh(other_mesh)) return;
            mergeShape(mesh, other_mesh, uv_2_element, uv_3_element,
                       invert_mesh_front_back);
            mesh.extendLastSubMesh(mesh.getIndices().size());

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

    void MeshMerger::mergePolygon(Mesh& mesh, const Polygon& other_poly, const MeshExtractOptions& mesh_extract_options,
                                  const GeoReference& geo_reference, const TVec2f& uv_2_element,
                                  const TVec2f&uv_3_element, const std::string& gml_path) {
        if (!isValidPolygon(other_poly)) return;
        // TODO ここで Polygon から Mesh に変換するためにデータのコピーが入るので重そう
        auto other_mesh = plateauPolygonToMesh(other_poly, mesh_extract_options.exclude_triangles_outside_extent, gml_path, mesh_extract_options.extent, geo_reference);
        mergeMesh(mesh, other_mesh, mesh_extract_options.mesh_axes, mesh_extract_options.export_appearance,
                  uv_2_element, uv_3_element);
    }

    void MeshMerger::mergeMesh(Mesh& mesh, const Mesh& other_mesh, CoordinateSystem mesh_axes, bool include_textures,
                               const TVec2f& uv_2_element, const TVec2f& uv_3_element){
        if(!isValidMesh(other_mesh)) return;
        bool invert_mesh_front_back = shouldInvertIndicesOnMeshConvert(mesh_axes);
        if(include_textures){
            mergeWithTexture(mesh, other_mesh, uv_2_element, uv_3_element, invert_mesh_front_back);
        }else{
            mergeWithoutTexture(mesh, other_mesh, uv_2_element, uv_3_element, invert_mesh_front_back);
        }
    }

    void MeshMerger::mergeMeshInfo(Mesh& mesh,
                                   std::vector<TVec3d>&& vertices, std::vector<unsigned>&& indices, UV&& uv_1,
                                   std::vector<SubMesh>&& sub_meshes,
                                   CoordinateSystem mesh_axes, bool include_texture){
        auto other_mesh = Mesh("", std::move(vertices), std::move(indices), std::move(uv_1), std::move(sub_meshes));
        mergeMesh(mesh, other_mesh, mesh_axes, include_texture, TVec2f(0, 0), TVec2f(0, 0));
    }

    void MeshMerger::mergePolygonsInCityObject(Mesh& mesh, const CityObject& city_object, unsigned int lod,
                                               const MeshExtractOptions& mesh_extract_options, const GeoReference& geo_reference,
                                               const TVec2f& uv_2_element, const TVec2f& uv_3_element, const std::string& gml_path) {
        auto polygons = findAllPolygons(city_object, lod);
        for (auto poly: polygons) {
            MeshMerger::mergePolygon(mesh, *poly, mesh_extract_options, geo_reference, uv_2_element, uv_3_element,
                                     gml_path);
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
