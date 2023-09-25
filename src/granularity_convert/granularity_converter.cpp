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

        /// MeshのうちCityObjectIndexが引数idに一致する箇所のみを取り出したMeshを生成して返します。
        Mesh filterByCityObjIndex(const Mesh& src, CityObjectIndex filter_id) {
            const auto& src_vertices = src.getVertices();
            const auto vertex_count = src_vertices.size();
            const auto& src_uv1 = src.getUV1();
            const auto& src_uv4 = src.getUV4();

            auto dst_vertices = std::vector<TVec3d>();
            auto dst_uv1 = std::vector<TVec2f>();
            auto dst_uv4 = std::vector<TVec2f>();
            dst_vertices.reserve(src.getVertices().size());
            dst_uv1.reserve(src.getUV1().size());
            dst_uv4.reserve(src.getUV4().size());

            // 不要頂点を削除して頂点番号を詰めたとき、i番目の頂点がnext_index[i]番目に移動するものとします。
            // ただし、i番目の頂点が削除されたとき、next_index[i] = -1 とします。
            // そのようなnext_indexを作ります。
            std::vector<long> next_index;
            next_index.reserve(vertex_count);
            std::size_t current_vert_id = 0;
            for(std::size_t i=0; i<vertex_count; i++){
                auto src_id = CityObjectIndex::fromUV(src_uv4.at(i));
                if( src_id == filter_id) {
                    next_index.push_back((long)current_vert_id);
                    dst_vertices.push_back(src_vertices.at(i));
                    dst_uv1.push_back(src_uv1.at(i));
                    dst_uv4.push_back(src_uv4.at(i));
                    ++current_vert_id;
                }else{
                    next_index.push_back(-1);
                }
            }

            // src_indicesについて、next_index[]を参考に、削除後の新たな頂点番号に置き換えたdst_indicesを作ります。
            const auto& src_indices = src.getIndices();
            auto dst_indices = std::vector<unsigned>();
            dst_indices.reserve(src_indices.size());
            for(auto src_index : src_indices) {
                const auto next_id = next_index.at(src_index); // 削除後の頂点番号
                if(next_id < 0) continue;
                dst_indices.push_back(next_id);
            }

            auto ret = Mesh();
            ret.addVerticesList(dst_vertices);
            ret.addIndicesList(dst_indices, 0, false);
            ret.setUV1(std::move(dst_uv1));
            ret.setUV4(std::move(dst_uv4));
            return ret;
        }

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
                    const auto mesh = src_node.getMesh();
                    if(mesh!=nullptr){

                    }
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

                        auto primary_mesh = filterByCityObjIndex(*mesh, CityObjectIndex(primary_id, -1));
                        if(primary_mesh.hasVertices()){
                            primary_node.setMesh(std::make_unique<Mesh>(primary_mesh));
                        }

                        // PrimaryIndex相当のノードの子に、AtomicIndex相当のノードを作ります。
                        for(const auto& id : indices_in_mesh) {
                            if(id.primary_index != primary_id) continue;
                            if(id.atomic_index < 0) continue;
                            auto atomic_node_tmp = Node(id.toString());
                            primary_node.addChildNode(std::move(atomic_node_tmp));
                            auto& atomic_node = primary_node.getLastChildNode();

                            auto atomic_mesh = filterByCityObjIndex(*mesh, id);
                            if(atomic_mesh.hasVertices()){
                                atomic_node.setMesh(std::make_unique<Mesh>(atomic_mesh));
                            }

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
