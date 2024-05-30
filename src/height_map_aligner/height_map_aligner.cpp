#include <plateau/height_map_alighner/height_map_aligner.h>
using namespace plateau::polygonMesh;


namespace plateau::heightMapAligner {
    namespace {

        void alignNode(Node& node, const HeightMapFrame& map) {
            auto mesh = node.getMesh();
            if(mesh == nullptr) return;
            auto& vertices = mesh->getVertices();
            for(auto& vertex : vertices) {
                vertex.y = map.posToHeight(TVec2d(vertex.x, vertex.z));
            }
        }

        void alignRecursive(Node& node, const HeightMapFrame& map) {
            alignNode(node, map);
            for(int i=0; i<node.getChildCount(); i++) {
                auto& child = node.getChildAt(i);
                alignRecursive(child, map);
            }
        }
    } // END 無名namespace

    void HeightMapAligner::align(Model& model, const HeightMapFrame& map) {
        for(int i=0; i<model.getRootNodeCount(); i++){
            auto& node = model.getRootNodeAt(i);
            alignRecursive(node, map);
        }
    }

    float HeightMapFrame::posToHeight(TVec2d pos) const {
        int map_x = (int)((pos.x - min_x) / (max_x - min_x) * map_width);
        int map_y = (int)((pos.y - min_y) / (max_y - min_y) * map_height);
        map_x = std::max(0, map_x);
        map_y = std::max(0, map_y);
        map_x = std::min(map_x, map_width - 1);
        map_y = std::min(map_y, map_height - 1);
        auto map_val = heightmap[map_y * map_width + map_x];
        return min_height + (max_height - min_height) * (float)map_val / std::numeric_limits<decltype(map_val)>::max();
    }
}
