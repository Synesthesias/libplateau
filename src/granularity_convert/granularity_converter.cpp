#include <plateau/granularity_convert/granularity_converter.h>
#include <queue>
#include <tuple>
#include <set>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    /// ノードを幅優先探索で処理するためのノードキューです。
    class NodeBFSQueue {
        using NodeT = std::tuple<const Node&, Node&>; // 変換前ノードと、それに対応する変換後ノードのタプル
    public:
        void push(const Node& src_node, Node& dst_node) {
            auto node = NodeT(src_node, dst_node);
            queue.push(node);
        };

        NodeT pop() {
            auto ret = queue.front();
            queue.pop();
            return ret;
        };

        bool empty() {
            return queue.empty();
        };

    private:
        std::queue<NodeT> queue;
    };


    namespace {

        Model convertToAtomic(const Model& src) {
            Model dst = Model();
            auto queue = NodeBFSQueue();

            // ルートノードをdstに作って探索キューに入れます。
            for(int i=0; i<src.getRootNodeCount(); i++) {
                auto& src_node = src.getRootNodeAt(i);
                auto dst_node = Node(src_node.getName());
                dst.addNode(std::move(dst_node));
                queue.push(src_node, dst.getRootNodeAt(i));
            }

            // 幅優先探索の順番で変換します。
            while(!queue.empty()) {
                auto [src_node, dst_node] = queue.pop();
                if(src_node.getMesh() != nullptr) {

                    // どのインデックスが含まれるかを列挙します。
                    auto mesh = src_node.getMesh();
                    std::set<CityObjectIndex> indices_in_mesh;
                    std::set<int> primary_indices_in_mesh;
                    for(const auto& uv4 : mesh->getUV4()) {
                        auto id = CityObjectIndex::fromUV(uv4);
                        indices_in_mesh.insert(id);
                        primary_indices_in_mesh.insert(id.primary_index);
                    }

                    // PrimaryIndexごとにノードを作ります。
                    for(auto primary_id : primary_indices_in_mesh) {
                        auto primary_node_tmp = Node(std::to_string(primary_id));
                        dst_node.addChildNode(std::move(primary_node_tmp));
                        auto& primary_node = dst_node.getLastChildNode();
                        // TODO ここにMesh追加
                        // PrimaryIndex相当のノードの子に、AtomicIndex相当のノードを作ります。
                        for(const auto& id : indices_in_mesh) {
                            if(id.primary_index != primary_id) continue;
                            if(id.atomic_index < 0) continue;
                            auto atomic_node_tmp = Node(id.toString());
                            primary_node.addChildNode(std::move(atomic_node_tmp));
                            auto& atomic_node = primary_node.getLastChildNode();
                            // TODO ここにMesh追加

                        }
                    }

                    // TODO ここに分解処理
                }

                // 子をキューに積みます。
                for(int i=0; i<src_node.getChildCount(); i++) {
                    auto& src_child = src_node.getChildAt(i);
                    auto dst_child = Node(src_child.getName());
                    queue.push(src_child, dst_child);
                }
            }

            return dst;
        }

        /// srcが最小地物単位であることを前提に、optionに沿ってポリゴンをマージします。
        Model merge(const Model& src, GranularityConvertOption option) {
            // TODO
            Model dst = Model();
            return dst;
        }
    }

    Model GranularityConverter::convert(const Model& src, GranularityConvertOption option) {
        // 組み合わせの数を減らすため、まず最小地物に変換してから望みの粒度に変換します。
        auto atomic = convertToAtomic(src);
//        auto converted = merge(atomic, option);
//        return converted;
        return atomic;
    }
}
