#include <plateau/granularity_convert/convert_from_atomic_to_primary.h>
#include <plateau/granularity_convert/node_queue.h>
#include "plateau/granularity_convert/merge_primary_node_and_children.h"

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    Model ConvertFromAtomicToPrimary::convert(Model* src) const {
        auto dst_model = Model();
        auto queue = NodeQueue();
        dst_model.reserveRootNodes(src->getRootNodeCount());
        // ルートノードを探索キューに加えると同時に、dst_modelに同じノードを準備します。
        for (int i = 0; i < src->getRootNodeCount(); i++) {
            const auto& src_node = src->getRootNodeAt(i);
            dst_model.addNode(Node(src_node.getName()));
            auto& dst_node = dst_model.getRootNodeAt(i);
            queue.push(NodePath({i}));
        }
        // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードの子を含めて結合します。そのprimary_idは0とします。
        while (!queue.empty()) {
            const auto node_path = queue.pop();
            auto src_node = node_path.toNode(src);
            auto dst_node = node_path.toNode(&dst_model);
            if (src_node->isPrimary()) {
                // Primaryなら、src_nodeとその子を結合したメッシュをdst_nodeに持たせます。
                auto dst_mesh = std::make_unique<Mesh>();
                MergePrimaryNodeAndChildren().merge(*src_node, *dst_mesh, 0);
                dst_node->setMesh(std::move(dst_mesh));
            } else {
                dst_node->reserveChild(dst_node->getChildCount() + src_node->getChildCount());
                // Primaryでないなら、子をキューに加えて探索を続けます。同じ子をdst_nodeに加えます。
                for (int i = 0; i < src_node->getChildCount(); i++) {
                    const auto& src_child = src_node->getChildAt(i);
                    auto& dst_child = dst_node->addChildNode(Node(src_child.getName()));
                    queue.push(node_path.plus(i));
                }
            }
        }
        return dst_model;
    }
}
