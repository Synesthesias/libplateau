#include <plateau/height_map_alighner/height_map_aligner.h>
#include <plateau/height_map_alighner/open_mesh_converter.h>
#include "OpenMesh/Tools/Subdivider/Uniform/LongestEdgeT.hh"

using namespace plateau::polygonMesh;
using namespace OpenMesh;


namespace plateau::heightMapAligner {


    namespace {

        void alignMesh(Mesh* mesh, const HeightMapFrame& map) {


            // OpenMeshのSubdivision機能を使いたいので、OpenMeshのメッシュを作成します。
            // OpenMeshについてはこちらを参照してください: https://www.graphics.rwth-aachen.de/media/openmesh_static/Documentations/OpenMesh-6.1-Documentation/a00046.html
            auto om_mesh = OpenMeshConverter().toOpenMesh(mesh);


            // OpenMeshでSubdivisionします
            auto divider = Subdivider::Uniform::LongestEdgeT<MeshType>();
            divider.attach(om_mesh);
            divider.set_max_edge_length(4); // ここでSubdivision後の最大エッジ長を指定します。
            divider(1); // 1回Subdivisionを実行
            divider.detach();


            // OpenMeshからMeshに直します
            OpenMeshConverter().toPlateauMesh(mesh, om_mesh);

            auto& vertices = mesh->getVertices();
            // 高さをハイトマップに合わせます
            for(auto& vertex : vertices) {
                vertex.y = map.posToHeight(TVec2d(vertex.x, vertex.z), 60 /* 気持ち高めで */);
            }
        }

        void alignNode(Node& node, const HeightMapFrame& map) {
            auto mesh = node.getMesh();
            if(mesh == nullptr) return;
            alignMesh(mesh, map);
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
