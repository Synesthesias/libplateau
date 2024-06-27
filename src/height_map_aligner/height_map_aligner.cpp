#include <plateau/height_map_alighner/height_map_aligner.h>
#include <plateau/height_map_alighner/open_mesh_converter.h>
#include <cmath>
#include <algorithm>

using namespace plateau::polygonMesh;
using namespace OpenMesh;


namespace plateau::heightMapAligner {


    namespace {

        /// どのハイトマップに合わせるか決めます。決め方は最初の頂点がどのハイトマップに近いかを見ます。
        const HeightMapFrame& chooseNearestMap(const std::vector<HeightMapFrame>& maps, const std::vector<TVec3d>& vertices) {
            int min_index = 0;
            float min_dist = std::numeric_limits<float>::max();
            for(int i=0; i<maps.size(); i++) {
                auto& m = maps[i];
                auto& v = vertices[0];
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

        void alignMesh(Mesh* mesh, const std::vector<HeightMapFrame>& maps, const double height_offset) {


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

            auto& map = chooseNearestMap(maps, vertices);

            // 高さをハイトマップに合わせます
            for(auto& vertex : vertices) {
                vertex.y = map.posToHeight(TVec2d(vertex.x, vertex.z), height_offset);
            }
        }

        void alignNode(Node& node, const std::vector<HeightMapFrame>& maps, const double height_offset) {
            auto mesh = node.getMesh();
            if(mesh == nullptr) return;
            alignMesh(mesh, maps, height_offset);
        }

        void alignRecursive(Node& node, const std::vector<HeightMapFrame>& maps, const double height_offset) {
            alignNode(node, maps, height_offset);
            for(int i=0; i<node.getChildCount(); i++) {
                auto& child = node.getChildAt(i);
                alignRecursive(child, maps, height_offset);
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

    void HeightMapAligner::align(Model& model) const {
        if(height_map_frames.empty()) throw std::runtime_error("HeightMapAligner::align: No height map frame added.");
        for(int i=0; i<model.getRootNodeCount(); i++){
            auto& node = model.getRootNodeAt(i);
            alignRecursive(node, height_map_frames, height_offset);
        }
    }

    double HeightMapFrame::posToHeight(const TVec2d pos, const double height_offset) const {
        // posがハイトマップ上の何ピクセル目に相当するか求めます。補間のため小数点以下も利用します。
        const auto map_x_raw = (pos.x - min_x) / (max_x - min_x) * map_width - 0.5; // rawは、まだ範囲内に収める処理をしていない値
        const auto map_y_raw = (pos.y - min_y) / (max_y - min_y) * map_height - 0.5;
        const auto map_x = std::max(0.0 , std::min(map_x_raw, (double)map_width - 1));
        const auto map_y = std::max(0.0 , std::min(map_y_raw, (double)map_height - 1));
        // 上の式で -0.5 する理由は、ピクセル数の基準点をもっとも左上のピクセルの左上から移動し、もっとも左上ピクセルのピクセルの中心にするためです。
        // ピクセルの値は、ピクセルの中心点の高さを表すと考えるための -0.5 です。

        // ハイトマップの値を読みます。補間のため注目点の周囲の4点を読みます。左上をp0とし、そこから時計回りにp1,p2,p3とします。
        const int x_floor = std::max((int)std::floor(map_x), 0);
        const int y_floor = std::max((int)std::floor(map_y), 0);
        const int x_ceil = std::min((int)std::ceil(map_x), map_width - 1);
        const int y_ceil = std::min((int)std::ceil(map_y), map_height - 1);
        const auto p0 = heightmap[y_floor * map_width + x_floor];
        const auto p1 = heightmap[y_floor * map_width + x_ceil];
        const auto p2 = heightmap[y_ceil * map_width + x_ceil];
        const auto p3 = heightmap[y_ceil * map_width + x_floor];

        // 補完します。2次元の線形補間です。
        const auto x_decimal_part = std::max(map_x - x_floor, 0.0);
        const auto y_decimal_part = std::max(map_y - y_floor, 0.0);
        const auto map_val =
                (MapValueT)lerp(
                        lerp(p0, p1, x_decimal_part),
                        lerp(p3, p2, x_decimal_part),
                        y_decimal_part
                );


        return min_height + (max_height - min_height) * (double)map_val / std::numeric_limits<MapValueT>::max() + height_offset;
    }
}
