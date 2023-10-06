#include <plateau/granularity_convert/convert_from_atomic_to_area.h>
#include <plateau/granularity_convert/node_queue.h>
#include <plateau/granularity_convert/merge_primary_node_and_children.h>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    Model ConvertFromAtomicToArea::convert(Model* src) {
        auto dst_model = Model();
        const auto root_node_name = src->getRootNodeCount() == 1 ?
                                    src->getRootNodeAt(0).getName() : "combined";
        auto dst_node_tmp = Node(root_node_name);
        dst_model.reserveRootNodes(src->getRootNodeCount());
        auto& dst_node = dst_model.addNode(std::move(dst_node_tmp));
        dst_node.setIsPrimary(true);
        dst_node.setMesh(std::make_unique<Mesh>());
        auto& dst_mesh = *dst_node.getMesh();

        // 探索用キュー
        auto queue = NodeQueue();

        // ルートノードを探索キューに加えます。
        for (int i = 0; i < src->getRootNodeCount(); i++) {
            queue.push(NodePath({i}));
        }

        // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードを子を含めて結合し、primary_idをインクリメントします。
        long primary_id = 0;
        while (!queue.empty()) {
            const auto node_path = queue.pop();
            const auto& src_node = node_path.toNode(src);
            if (src_node->isPrimary()) {
                // PrimaryなNodeが見つかったら、そのノードと子をマージします。
                MergePrimaryNodeAndChildren().merge(*src_node, dst_mesh, primary_id);
                primary_id++;
            } else {
                // PrimaryなNodeでなければ、Primaryに行き着くまで探索を続けます。
                dst_node.reserveChild(src_node->getChildCount());
                for (int i = 0; i < src_node->getChildCount(); i++) {
                    queue.push(node_path.plus(i));
                }
            }
        }
        return dst_model;
    }
}
