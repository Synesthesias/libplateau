#include <plateau/height_map_alighner/height_map_aligner.h>
#include <plateau/height_map_alighner/open_mesh_converter.h>

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

        void alignMesh(Mesh* mesh, const std::vector<HeightMapFrame>& maps) {


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
                vertex.y = map.posToHeight(TVec2d(vertex.x, vertex.z), 60 /* 気持ち高めで */);
            }
        }

        void alignNode(Node& node, const std::vector<HeightMapFrame>& maps) {
            auto mesh = node.getMesh();
            if(mesh == nullptr) return;
            alignMesh(mesh, maps);
        }

        void alignRecursive(Node& node, const std::vector<HeightMapFrame>& maps) {
            alignNode(node, maps);
            for(int i=0; i<node.getChildCount(); i++) {
                auto& child = node.getChildAt(i);
                alignRecursive(child, maps);
            }
        }
    } // END 無名namespace

    void HeightMapAligner::AddHeightmapFrame(const HeightMapFrame& heightmap_frame) {
        height_map_frames.push_back(heightmap_frame);
    }

    void HeightMapAligner::align(Model& model) const {
        if(height_map_frames.empty()) throw std::runtime_error("HeightMapAligner::align: No height map frame added.");
        for(int i=0; i<model.getRootNodeCount(); i++){
            auto& node = model.getRootNodeAt(i);
            alignRecursive(node, height_map_frames);
        }
    }

    float HeightMapFrame::posToHeight(TVec2d pos, float offset_map) const {
        int map_x = (int)((pos.x - min_x) / (max_x - min_x) * map_width);
        int map_y = (int)((pos.y - min_y) / (max_y - min_y) * map_height);
        map_x = std::max(0, map_x);
        map_y = std::max(0, map_y);
        map_x = std::min(map_x, map_width - 1);
        map_y = std::min(map_y, map_height - 1);
        auto map_val = heightmap[map_y * map_width + map_x];
        return min_height + (max_height - min_height) * (float)(map_val + offset_map) / std::numeric_limits<decltype(map_val)>::max();
    }
}
