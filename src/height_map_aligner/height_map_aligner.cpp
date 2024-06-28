#include <plateau/height_map_alighner/height_map_aligner.h>
#include <plateau/height_map_alighner/open_mesh_converter.h>
#include <plateau/height_map_generator/heightmap_generator.h>
#include <cmath>
#include <algorithm>
#include "plateau/geometry/geo_coordinate.h"

using namespace plateau::polygonMesh;
using namespace OpenMesh;


namespace plateau::heightMapAligner {


    namespace {
        using namespace heightMapGenerator;
        using namespace plateau::geometry;

        /// どのハイトマップに合わせるか決めます。決め方は最初の頂点がどのハイトマップに近いかを見ます。
        HeightMapFrame& chooseNearestMap(std::vector<HeightMapFrame>& maps, TVec3d vertex) {
            int min_index = 0;
            float min_dist = std::numeric_limits<float>::max();
            for(int i=0; i<maps.size(); i++) {
                auto& m = maps[i];
                auto& v = vertex;
                float dist_x;
                float dist_y;

                // x
                if(v.x < m.min_x) dist_x = m.min_x - v.x;
                else if(v.x > m.max_x) dist_x = v.x - m.max_x;
                else dist_x = 0;

                // y
                if(v.z < m.min_y) dist_y = m.min_y - v.z;
                else if(v.z > m.max_y) dist_y = v.z - m.max_y;
                else dist_y = 0;

                // 距離
                float dist = std::sqrt(dist_x * dist_x + dist_y * dist_y);
                if(dist < min_dist) {
                    min_dist = dist;
                    min_index = i;
                }
            }
            auto& map = maps[min_index];
            return map;
        }

        void alignMesh(Mesh* mesh, std::vector<HeightMapFrame>& maps, const double height_offset) {


            // OpenMeshのSubdivision機能を使いたいので、OpenMeshのメッシュを作成します。
            // OpenMeshについてはこちらを参照してください: https://www.graphics.rwth-aachen.de/media/openmesh_static/Documentations/OpenMesh-6.1-Documentation/a00046.html
            auto converter = OpenMeshConverter();
            auto om_mesh = converter.toOpenMesh(mesh);

            // OpenMeshでSubdivisionします
            converter.subdivide(om_mesh);


            // OpenMeshからMeshに直します
            converter.toPlateauMesh(mesh, om_mesh);

            mesh->combineSameSubMeshes();

            auto& vertices = mesh->getVertices();

            const auto& map = chooseNearestMap(maps, vertices[0]);

            // 高さをハイトマップに合わせます
            for(auto& vertex : vertices) {
                vertex.y = map.posToHeight(TVec2d(vertex.x, vertex.z), height_offset);
            }
        }

        void alignMeshInvert(Mesh* mesh, std::vector<HeightMapFrame>& maps) {

            // 与えられたメッシュだけのハイトマップを作ります。
            // この際、範囲などの条件を地形ハイトマップと同じに設定することで、後で比較できるようにします。
            auto& vertices = mesh->getVertices();
            if(vertices.empty()) return;
            auto& land_map = chooseNearestMap(maps, vertices.at(0));
            constexpr CoordinateSystem coordinate = CoordinateSystem::EUN;
            auto mesh_triangles = TriangleList::generateFromMesh(mesh->getIndices(), vertices, coordinate);
            auto land_extent = HeightMapExtent();
            land_extent.Min = TVec3d(land_map.min_x, land_map.min_y, land_map.min_height); // EUN to ENU
            land_extent.Max = TVec3d(land_map.max_x, land_map.max_y, land_map.max_height); // EUN to ENU

            mesh_triangles.Extent = land_extent;
            auto mesh_map = HeightmapGenerator().generateFromMeshAndTriangles(
                    *mesh, land_map.map_width, land_map.map_height, false, mesh_triangles
                    );

            // 同じ条件で作られた地形のハイトマップとメッシュのハイトマップを比較し、地形マップを修正します。
            for(int y=0; y<land_map.map_height; y++) {
                for(int x=0; x<land_map.map_width; x++) {
                    int index = y * land_map.map_width + x;
                    if(!mesh_map.alpha_map.at(index)) continue;
                    const auto mesh_map_val = mesh_map.height_map.at(index);
                    land_map.heightmap.at(index) = mesh_map_val;
                }
            }
        }

        void alignNode(Node& node, std::vector<HeightMapFrame>& maps, const double height_offset) {
            auto mesh = node.getMesh();
            if(mesh == nullptr) return;
            alignMesh(mesh, maps, height_offset);
        }

        void alignNodeInvert(Node& node, std::vector<HeightMapFrame>& maps) {
            auto mesh = node.getMesh();
            if(mesh == nullptr) return;
            alignMeshInvert(mesh, maps);
        }

        void alignRecursive(Node& node, std::vector<HeightMapFrame>& maps, const double height_offset) {
            alignNode(node, maps, height_offset);
            for(int i=0; i<node.getChildCount(); i++) {
                auto& child = node.getChildAt(i);
                alignRecursive(child, maps, height_offset);
            }
        }

        void alignRecursiveInvert(Node& node, std::vector<HeightMapFrame>& maps) {
            alignNodeInvert(node, maps);
            for(int i=0; i<node.getChildCount(); i++) {
                auto& child = node.getChildAt(i);
                alignRecursiveInvert(child, maps);
            }
        }

        double lerp(const double a, const double b, const double t) {
            const double saturated = std::min(1.0, std::max(0.0, t));
            return a + saturated * (b - a);
        }
    } // END 無名namespace

    void HeightMapAligner::addHeightmapFrame(const HeightMapFrame& heightmap_frame) {
        height_map_frames.push_back(heightmap_frame);
    }

    void HeightMapAligner::align(Model& model) {
        if(height_map_frames.empty()) throw std::runtime_error("HeightMapAligner::align: No height map frame added.");
        for(int i=0; i<model.getRootNodeCount(); i++){
            auto& node = model.getRootNodeAt(i);
            alignRecursive(node, height_map_frames, height_offset);
        }
    }

    void HeightMapAligner::alignInvert(plateau::polygonMesh::Model& model) {
        if(height_map_frames.empty()) throw std::runtime_error("HeightMapAligner::alignInvert: No height map frame added.");
        for(int i=0; i<model.getRootNodeCount(); i++){
            auto& node = model.getRootNodeAt(i);
            alignRecursiveInvert(node, height_map_frames);
        }
    }

    int HeightMapAligner::heightmapCount() const {
        return (int)height_map_frames.size();
    }

    HeightMapFrame& HeightMapAligner::getHeightMapFrameAt(int index) {
        return height_map_frames.at(index);
    }

    double HeightMapFrame::posToHeight(const TVec2d pos, const double height_offset) const {
        const auto map_pos = posToMapPos(pos);

        // ハイトマップの値を読みます。補間のため注目点の周囲の4点を読みます。左上をp0とし、そこから時計回りにp1,p2,p3とします。
        const int x_floor = std::max((int)std::floor(map_pos.x), 0);
        const int y_floor = std::max((int)std::floor(map_pos.y), 0);
        const int x_ceil = std::min((int)std::ceil(map_pos.x), map_width - 1);
        const int y_ceil = std::min((int)std::ceil(map_pos.y), map_height - 1);
        const auto p0 = heightmap[y_floor * map_width + x_floor];
        const auto p1 = heightmap[y_floor * map_width + x_ceil];
        const auto p2 = heightmap[y_ceil * map_width + x_ceil];
        const auto p3 = heightmap[y_ceil * map_width + x_floor];

        // 補完します。2次元の線形補間です。
        const auto x_decimal_part = std::max(map_pos.x - x_floor, 0.0);
        const auto y_decimal_part = std::max(map_pos.y - y_floor, 0.0);
        const auto map_val =
                (MapValueT)lerp(
                        lerp(p0, p1, x_decimal_part),
                        lerp(p3, p2, x_decimal_part),
                        y_decimal_part
                );


        return min_height + (max_height - min_height) * (((double)map_val) + 0.5) / ((double)std::numeric_limits<MapValueT>::max()) + height_offset;
    }

    TVec2d HeightMapFrame::posToMapPos(const TVec2d& pos) const {
        // posがハイトマップ上の何ピクセル目に相当するか求めます。補間のため小数点以下も利用します。
        const auto map_x_raw = (pos.x - min_x) / (max_x - min_x) * map_width - 0.5; // rawは、まだ範囲内に収める処理をしていない値
        const auto map_y_raw = (pos.y - min_y) / (max_y - min_y) * map_height - 0.5;
        const auto map_x = std::max(0.0 , std::min(map_x_raw, (double)map_width - 1));
        const auto map_y = std::max(0.0 , std::min(map_y_raw, (double)map_height - 1));
        // 上の式で -0.5 する理由は、ピクセル数の基準点をもっとも左上のピクセルの左上から移動し、もっとも左上ピクセルのピクセルの中心にするためです。
        // ピクセルの値は、ピクセルの中心点の高さを表すと考えるための -0.5 です。
        return {map_x, map_y};
    }
}
