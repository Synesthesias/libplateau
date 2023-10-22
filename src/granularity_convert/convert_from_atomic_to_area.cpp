#include <plateau/granularity_convert/convert_from_atomic_to_area.h>
#include <plateau/granularity_convert/node_queue.h>
#include <plateau/granularity_convert/merge_primary_node_and_children.h>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    Model ConvertFromAtomicToArea::convert(const Model* src) const {
        auto dst_model = Model();
        const auto root_node_name = src->getRootNodeCount() == 1 ?
                                    src->getRootNodeAt(0).getName() : "combined";
        dst_model.reserveRootNodes(src->getRootNodeCount());
        auto& dst_node = dst_model.addNode(Node(root_node_name));
        dst_node.setIsPrimary(true);
        dst_node.setMesh(std::make_unique<Mesh>());
        auto& dst_mesh = *dst_node.getMesh();

        // 探索用キュー
        auto queue = NodeQueue();
        queue.pushRoot(src);

        // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードを子を含めて結合し、primary_idをインクリメントします。
        long primary_id = -1;
        std::vector<NodePath> unmerged_nodes;
        std::string last_primary_gml_id = "dummy__";
        int atomic_id_offset = 0;
        while (!queue.empty()) {
            const auto node_path = queue.pop();
            const auto& src_node = node_path.toNode(src);

            bool src_contains_primary = src_node->isPrimary();
            auto src_mesh = src_node->getMesh();
            if(src_mesh != nullptr) {
                auto& src_city_obj_list = src_mesh->getCityObjectList();
                src_contains_primary |= !src_city_obj_list.getAllPrimaryIndices().empty();
            }

            if (src_contains_primary) {

                bool should_increment_primary_id = false;
                if(src_node->getMesh() != nullptr) {
                    auto& src_city_obj_list = src_node->getMesh()->getCityObjectList();
                    auto primaries = src_city_obj_list.getAllPrimaryIndices();
                    if(primaries.size() > 0) {
                        auto& primary_gml_id = src_city_obj_list.getPrimaryGmlID(primaries.at(0).primary_index);
                        if(primary_gml_id != last_primary_gml_id){
                            should_increment_primary_id = true;
                            atomic_id_offset = 0;
                            last_primary_gml_id = primary_gml_id;
                        }else{
                            atomic_id_offset++;
                        }
                    }
                }
                if(src_node->isPrimary()) {
                    should_increment_primary_id = true;
                }
                if(should_increment_primary_id) primary_id++;

                // PrimaryなNodeが見つかったら、そのノードと子をマージします。
                MergePrimaryNodeAndChildren().mergeWithChildren(*src_node, dst_mesh, primary_id, atomic_id_offset);

            } else {
                // PrimaryなNodeでなければ、Primaryに行き着くまで探索を続けます。
                dst_node.reserveChild(src_node->getChildCount());
                for (int i = 0; i < src_node->getChildCount(); i++) {
                    queue.push(node_path.plus(i));
                }
                // Primaryが不明なAtomic
                if(src_node->getMesh() != nullptr) {
                    unmerged_nodes.push_back(node_path);
                }
            }
        }

        // Primary Nodeが不明なAtomicをマージします。
//        int atomic_id = 0;
//        for(const auto& unmerged_node : unmerged_nodes) {
//            const auto src_mesh = unmerged_node.toNode(src)->getMesh();
//            MergePrimaryNodeAndChildren().merge(*src_mesh, dst_mesh, CityObjectIndex(primary_id, atomic_id));
//            auto& dst_city_obj_list = dst_mesh.getCityObjectList();
//            // 主要地物のGML IDを追加
//            if(!dst_city_obj_list.containsCityObjectIndex(CityObjectIndex(primary_id, -1))){
//                auto& src_city_obj_list = src_mesh->getCityObjectList();
//                auto primaries = src_city_obj_list.getAllPrimaryIndices();
//                for(auto& primary : primaries) {
//                    auto& primary_gml_id = src_city_obj_list.getPrimaryGmlID(primary.primary_index);
//                    dst_city_obj_list.add(primary, primary_gml_id);
//                }
//            }
//            atomic_id++;
//        }
        return dst_model;
    }
}
