#include <plateau/granularity_convert/convert_from_atomic_to_primary.h>
#include <plateau/granularity_convert/node_queue.h>
#include "plateau/granularity_convert/merge_primary_node_and_children.h"

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    Model ConvertFromAtomicToPrimary::convert(const Model* src) const {
        auto dst_model = Model();
        auto queue = NodeQueue();
        dst_model.reserveRootNodes(src->getRootNodeCount());
        // ルートノードを探索キューに加えると同時に、dst_modelに同じノードを準備します。
        for (int i = 0; i < src->getRootNodeCount(); i++) {
            const auto& src_node = src->getRootNodeAt(i);
            dst_model.addNode(Node(src_node.getName()));
            queue.push(NodePath({i}));
        }

        std::vector<NodePath> unmerged_atomics = {};
        // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードの子を含めて結合します。そのprimary_idは0とします。
        while (!queue.empty()) {
            auto node_path = queue.pop();
            auto src_node = node_path.toNode(src);
            auto dst_node = node_path.toNode(&dst_model);
            if (src_node->isPrimary()) {
                // Primaryなら、src_nodeとその子を結合したメッシュをdst_nodeに持たせます。
                auto dst_mesh = std::make_unique<Mesh>();
                MergePrimaryNodeAndChildren().mergeWithChildren(*src_node, *dst_mesh, 0);
                dst_node->setMesh(std::move(dst_mesh));
            } else {
                dst_node->reserveChild(dst_node->getChildCount() + src_node->getChildCount());
                // Primaryに行き着く前にメッシュが見つかった場合、未マージとして保持しておきます。
                // 入力のメッシュがすべて最小地物である場合にありえます。
                if (src_node->getMesh() != nullptr) {
                    unmerged_atomics.push_back(node_path);
                } else {
                    for (int i = 0; i < src_node->getChildCount(); i++) {

                        // 子をキューに加えて探索を続けます。同じ子をdst_nodeに加えます。
                        const auto& src_child = src_node->getChildAt(i);
                        dst_node->addChildNode(Node(src_child.getName()));
                        queue.push(node_path.plus(i));
                    }
                }
            }
        }

        // FIXME 未マージのものは、とりあえずまとめてルートノード1つにしておきますがもっと適切な方法があるかも……
        auto combined_mesh = std::make_unique<Mesh>();
        int atomic_id = 0;
        for(const auto& unmerged_path : unmerged_atomics) {
            auto src_mesh = unmerged_path.toNode(src)->getMesh();
//            auto src_node = unmerged_path.toNode(src);
//            MergePrimaryNodeAndChildren().mergeWithChildren(*src_node, *combined_mesh, 0);
            MergePrimaryNodeAndChildren().merge(*src_mesh, *combined_mesh, CityObjectIndex(0, atomic_id));
            atomic_id++;
//            combined_mesh->merge(*src_mesh, false, true);
//            auto& src_city_obj_list = src_mesh->getCityObjectList();
//            for(auto& [city_obj_index, gml_id] : src_city_obj_list) {
//                combined_mesh->getCityObjectList().add(city_obj_index, gml_id);
//            }
        }
        if(combined_mesh->hasVertices()) {
            auto combined_node = Node("combined");
            combined_node.setMesh(std::move(combined_mesh));
            dst_model.addNode(std::move(combined_node));
        }

        dst_model.eraseEmptyNodes();
        return dst_model;
    }
}
