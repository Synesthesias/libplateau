#include <plateau/granularity_convert/granularity_converter.h>
#include <plateau/granularity_convert/node_queue.h>
#include <plateau/granularity_convert/convert_to_atomic.h>
#include <plateau/granularity_convert/convert_from_atomic_to_primary.h>
#include <plateau/granularity_convert/merge_primary_node_and_children.h>
#include <tuple>
#include <set>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;


    namespace {


        /// 主要地物のノードとその子ノードを結合したものを、引数dst_meshに格納します。
        /// 引数に渡されるsrc_node_argはPrimaryであることを前提とします。


        /// 最小地物単位のモデルを受け取り、それを地域単位に変換したモデルを返します。
        Model convertFromAtomicToArea(Model& src) {
            auto dst_model = Model();
            const auto root_node_name = src.getRootNodeCount() == 1 ?
                                        src.getRootNodeAt(0).getName() : "combined";
            auto dst_node_tmp = Node(root_node_name);
            dst_model.reserveRootNodes(src.getRootNodeCount());
            auto& dst_node = dst_model.addNode(std::move(dst_node_tmp));
            dst_node.setIsPrimary(true);
            dst_node.setMesh(std::make_unique<Mesh>());
            auto& dst_mesh = *dst_node.getMesh();

            // 探索用キュー
            auto queue = NodeQueue();

            // ルートノードを探索キューに加えます。
            for (int i = 0; i < src.getRootNodeCount(); i++) {
                queue.push(NodePath({i}));
            }

            // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードを子を含めて結合し、primary_idをインクリメントします。
            long primary_id = 0;
            while (!queue.empty()) {
                const auto node_path = queue.pop();
                const auto& src_node = node_path.toNode(&src);
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


        Model convertFromAtomicToPrimary(Model& src_model) {

        }
    }

    Model GranularityConverter::convert(Model& src, GranularityConvertOption option) {
        // 組み合わせの数を減らすため、まず最小地物に変換してから望みの粒度に変換します。

        // 例：入力のNode構成が次のようだったとして、以下にその変化を例示します。
        // 入力： gml_node <- lod_node <- group_node

        auto atomic = ConvertToAtomic().convert(&src);

        // 例：上の行の実行後、次のようなNode構成になります。
        // gml_node <- lod_node <- primary_node <- atomic_node

        atomic.eraseEmptyNodes();

        switch (option.granularity_) {
            case MeshGranularity::PerAtomicFeatureObject:
                return atomic;
            case MeshGranularity::PerPrimaryFeatureObject:
                return ConvertFromAtomicToPrimary().convert(&atomic);
            case MeshGranularity::PerCityModelArea:
                return convertFromAtomicToArea(atomic);
            default:
                throw std::runtime_error("unknown argument");
        }
    }
}
