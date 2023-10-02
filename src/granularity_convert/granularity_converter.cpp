#include <plateau/granularity_convert/granularity_converter.h>
#include <queue>
#include <tuple>
#include <set>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    /// ノードを幅優先探索で処理するためのノードキューです。
    class NodeBFSQueue {
        using NodeT = std::tuple<const Node*, Node*>; // 変換前ノードと、それに対応する変換後ノードのタプル
    public:
        void push(const Node* src_node, Node* dst_node) {
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

        /**
         * ノード木構造の探索用キューです。
         * ただし、キューに入るノードを表現する型は、そのノードを直接入れるのではなく、親ノードとその子インデックスによって表現されます。
         * この理由は、子ノードのポインタを保持していると、ノードvectorの再割り当てが起きたときにポインタが外れるからです。
         * 自身ノードの代わりに親ノードを保持しておけば、親から順に子を組み替えていくという条件下では再割り当てに対応可能です。
         * なおルートノードを表現するときはparent_nodeをnullptrにするものとします。
         */
        class NodeQueueOfIndexOfParent{
            using TElem = std::tuple<Node*, int>;
        public:

            void push(Node* parent_node, int child_index) {
                queue_.push({parent_node, child_index});
            }

            TElem pop() {
                auto ret = queue_.front();
                queue_.pop();
                return ret;
            }

            bool empty() {
                return queue_.empty();
            }

            static Node* getNodeFromParent(Node* parent_node, int child_index, Model& model) {
                if(parent_node == nullptr) {
                    return &model.getRootNodeAt(child_index);
                }
                return &parent_node->getChildAt(child_index);
            }

        private:
            std::queue<TElem> queue_;
        };

        /**
         * メッシュを含むノードを親に移動します。
         * 例：
         * before: gml_node <- lod_node <- group_node <- primary_node <- atomic_node
         * after:  gml_node <- lod_node <- primary_node <- atomic_node
         */
        void movePrimaryNodesToParent(Model& model) {
            // 幅優先探索の探索キューです。
            auto queue = NodeQueueOfIndexOfParent();

            // ルートノードをキューに加えます。
            for(int i=0; i<model.getRootNodeCount(); i++) {
                queue.push(nullptr, i);
            }

            while(!queue.empty()) {
                auto [parent_node, child_index] = queue.pop();

                // ルートノードの場合は親がないので子を移動させません。
                if(parent_node != nullptr) {

                    // 子ノードのうち、主要地物に相当するものを親ノードに移動します。
                    // ここで親に移動したノードは、以降の探索から外れます。幅優先探索のため、親は探索済みとみなされます。
                    // なお、ループの中では毎回current_nodeを取得する必要があります。
                    // current_nodeを使い回してしまうと、親にノードが増えたとき、vectorの再割り当てによってポインタが無効になることがあります。
                    std::set<int> child_indices_to_delete;
                    for(int i=0; i<NodeQueueOfIndexOfParent::getNodeFromParent(parent_node, child_index, model)->getChildCount(); i++) {
                        auto current_node = NodeQueueOfIndexOfParent::getNodeFromParent(parent_node, child_index, model);
                        auto child_node = &current_node->getChildAt(i);
                        if(child_node->isPrimary()) {
                            parent_node->addChildNode(std::move(*child_node));
                            child_indices_to_delete.insert(i);
                        }
                    }
                    // 子ノードのうち、親に移動したものを除いた新しい子ノードvectorを作ります。
                    // fixme : 以下の新しいvector<Node>を作る処理は、削除がなければ必要がないので軽量化可能
                    std::vector<Node> next_child_nodes;
                    auto current_node = NodeQueueOfIndexOfParent::getNodeFromParent(parent_node, child_index, model);
                    for(int i=0; i<current_node->getChildCount(); i++) {
                        bool is_deleted = child_indices_to_delete.find(i) != child_indices_to_delete.end();
                        if(!is_deleted) {
                            auto* child_node = &current_node->getChildAt(i);
                            next_child_nodes.push_back(std::move(*child_node));
                        }
                    }
                    current_node->setChildNodes(std::move(next_child_nodes));
                }

                auto current_node = NodeQueueOfIndexOfParent::getNodeFromParent(parent_node, child_index, model);
                // 子をキューに加えます。
                for(int i=0; i<current_node->getChildCount(); i++) {
                    queue.push(current_node, i);
                }
            }


        }

        /// MeshのうちCityObjectIndexが引数idに一致する箇所のみを取り出したMeshを生成して返します。
        Mesh filterByCityObjIndex(const Mesh& src, CityObjectIndex filter_id) {
            const auto& src_vertices = src.getVertices();
            const auto vertex_count = src_vertices.size();
            const auto& src_uv1 = src.getUV1();
            const auto& src_uv4 = src.getUV4();
            const auto& src_sub_meshes = src.getSubMeshes();

            auto dst_vertices = std::vector<TVec3d>();
            auto dst_uv1 = std::vector<TVec2f>();
            auto dst_uv4 = std::vector<TVec2f>();
            dst_vertices.reserve(src.getVertices().size());
            dst_uv1.reserve(src.getUV1().size());
            dst_uv4.reserve(src.getUV4().size());

            // 不要頂点を削除して頂点番号を詰めたとき、i番目の頂点がvert_id_transform[i]番目に移動するものとします。
            // ただし、i番目の頂点が削除されたとき、vert_id_transform[i] = -1 とします。
            // そのようなvert_id_transformを作ります。
            std::vector<long> vert_id_transform;
            vert_id_transform.reserve(vertex_count);
            std::size_t current_vert_id = 0;
            for(std::size_t i=0; i<vertex_count; i++) {
                auto src_id = CityObjectIndex::fromUV(src_uv4.at(i));
                if( src_id == filter_id) {
                    vert_id_transform.push_back((long)current_vert_id);
                    dst_vertices.push_back(src_vertices.at(i));
                    dst_uv1.push_back(src_uv1.at(i));
                    dst_uv4.emplace_back(0, 0); // 最小地物単位にすると、CityObjectIndexはすべて(0, 0)になります。なぜなら、最小地物単位とはUV4によるメッシュ内区別が不要になるほど細かくノードを分割したものだからです。
                    ++current_vert_id;
                }else{
                    vert_id_transform.push_back(-1);
                }
            }

            // src_indicesについて、vert_id_transform[]を参考に、削除頂点を詰めたあとの新たな頂点番号に置き換えたdst_indicesを作ります。
            // 同時に、次のようなindices_id_transformを作ります。これはのちのSubMesh生成で利用します。
            // src_indicesのi番目が、頂点削除によってdst_indicesのj番目に移動するとき、
            // indices_id_transform.at(i) = j
            // ただし、src_indicesのi番目が削除されたとき
            // indices_id_transform.at(i) = -1
            // となるvector
            const auto& src_indices = src.getIndices();
            auto dst_indices = std::vector<unsigned>();
            auto indices_id_transform = std::vector<long>();
            dst_indices.reserve(src_indices.size());
            indices_id_transform.reserve(src_indices.size());
            for(auto src_index : src_indices) {
                const auto next_id = vert_id_transform.at(src_index); // 削除頂点を詰めたあとの新たな頂点番号
                if(next_id < 0) {
                    indices_id_transform.push_back(-1);
                    continue;
                }
                dst_indices.push_back(next_id);
                indices_id_transform.push_back((long)dst_indices.size()-1);
            }

            // SubMeshについて、元から削除部分を除いたvector<SubMesh>を生成します。
            auto dst_sub_meshes = std::vector<SubMesh>();
            for(const auto& src_sub_mesh : src_sub_meshes) {
                auto src_start = src_sub_mesh.getStartIndex();
                auto src_end = src_sub_mesh.getEndIndex();

                // src_startが削除されなかった部分にヒットするまで右に移動します。
                while(indices_id_transform.at(src_start) < 0){
                    src_start++;
                    if(src_start > src_end) break; // src_startからsrc_endまでの範囲がすべて削除されていたケース
                }
                if(src_start > src_end) continue;

                // src_endが削除されなかった部分にヒットするまで左に移動します。
                while(indices_id_transform.at(src_end) < 0) {
                    src_end--;
                    if(src_end < src_start) break; // src_startからsrc_endまでの範囲がすべて削除されていたケース
                }
                if(src_end < src_start) break;

                // 新しいSubMeshです。
                auto dst_start = indices_id_transform.at(src_start);
                auto dst_end = indices_id_transform.at(src_end);
                auto dst_sub_mesh = src_sub_mesh;
                dst_sub_mesh.setStartIndex((int)dst_start);
                dst_sub_mesh.setEndIndex((int)dst_end);
                dst_sub_meshes.push_back(dst_sub_mesh);
            }

            auto ret = Mesh();
            ret.addVerticesList(dst_vertices);
            ret.addIndicesList(dst_indices, 0, false);
            ret.setUV1(std::move(dst_uv1));
            ret.setUV4(std::move(dst_uv4));
            ret.setSubMeshes(dst_sub_meshes);
            return ret;
        }

        /// モデルを最小地物単位に変換します。引数の結合単位は問いません。
        Model convertToAtomic(const Model& src) {
            Model dst_model = Model();
            auto queue = NodeBFSQueue();

            // reserveを忘れると、vectorの再割り当てによってキューの参照が壊れる点に注意してください。ここ以下のノード追加する処理はすべてreserveを考慮する必要があります。
            dst_model.reserveRootNodes(src.getRootNodeCount());

            // ルートノードをdstに作って探索キューに入れます。
            for(int i=0; i<src.getRootNodeCount(); i++) {
                auto& src_node = src.getRootNodeAt(i);
                auto dst_node = Node(src_node.getName());
                dst_model.addNode(std::move(dst_node));
                queue.push(&src_node, &dst_model.getRootNodeAt(i));
            }

            // 幅優先探索の順番で変換します。
            while(!queue.empty()) {
                auto [src_node, dst_node] = queue.pop();
                if(src_node->getMesh() != nullptr) {

                    // どのインデックスが含まれるかを列挙します。
                    const auto src_mesh = src_node->getMesh();
                    std::set<CityObjectIndex> indices_in_mesh;
                    std::set<int> primary_indices_in_mesh;
                    for(const auto& uv4 : src_mesh->getUV4()) {
                        auto id = CityObjectIndex::fromUV(uv4);
                        indices_in_mesh.insert(id);
                        primary_indices_in_mesh.insert(id.primary_index);
                    }

                    const auto& src_city_obj_list = src_mesh->getCityObjectList();

                    // PrimaryIndexごとにノードを作ります。
                    dst_node->reserveChild(primary_indices_in_mesh.size() + src_node->getChildCount());
                    for(auto primary_id : primary_indices_in_mesh) {
                        std::string primary_gml_id = "gml_id_not_found";
                        src_city_obj_list.tryGetPrimaryGmlID(primary_id, primary_gml_id);
                        auto primary_node_tmp = Node(primary_gml_id);
                        primary_node_tmp.setIsPrimary(true);
                        auto& primary_node = dst_node->addChildNode(std::move(primary_node_tmp));

                        auto primary_mesh = filterByCityObjIndex(*src_mesh, CityObjectIndex(primary_id, -1));
                        if(primary_mesh.hasVertices()){
                            primary_mesh.setCityObjectList({{{{0, 0}, primary_gml_id}}});
                            primary_node.setMesh(std::make_unique<Mesh>(primary_mesh));
                        }

                        // PrimaryIndex相当のノードの子に、AtomicIndex相当のノードを作ります。
                        primary_node.reserveChild(indices_in_mesh.size());
                        for(const auto& id : indices_in_mesh) {
                            if(id.primary_index != primary_id) continue;
                            if(id.atomic_index == CityObjectIndex::invalidIndex()) continue;
                            std::string atomic_gml_id = "gml_id_not_found";
                            src_city_obj_list.tryGetAtomicGmlID(id, atomic_gml_id);
                            auto atomic_node_tmp = Node(atomic_gml_id);
                            auto& atomic_node = primary_node.addChildNode(std::move(atomic_node_tmp));

                            auto atomic_mesh = filterByCityObjIndex(*src_mesh, id);
                            if(atomic_mesh.hasVertices()){
                                atomic_mesh.setCityObjectList({{{{0, 0}, atomic_gml_id}}});
                                atomic_node.setMesh(std::make_unique<Mesh>(atomic_mesh));
                            }
                        }
                    }

                }

                // 子をキューに積みます。
                dst_node->reserveChild(src_node->getChildCount());
                for(int i=0; i<src_node->getChildCount(); i++) {
                    auto& src_child = src_node->getChildAt(i);
                    auto dst_child_tmp = Node(src_child.getName());
                    auto& dst_child = dst_node->addChildNode(std::move(dst_child_tmp));
                    queue.push(&src_child, &dst_child);
                }
            }

            return dst_model;
        }

        /// 主要地物のノードとその子ノードを結合したものを、引数dst_meshに格納します。
        void mergePrimaryNodeAndChildren(const Node& src_node_arg, Mesh& dst_mesh, int primary_id) {
            std::queue<const Node*> queue;
            queue.push(&src_node_arg);
            long next_atomic_id = 0;

            while(!queue.empty()){
                const auto& src_node = *queue.front();
                queue.pop();

                // メッシュをマージします。
                if(src_node.getMesh() != nullptr) {
                    // 元メッシュをコピーします。重いので注意してください。
                    auto src_mesh_copy = Mesh(*src_node.getMesh());
                    // UV4を置き換えます。
                    int atomic_id;
                    if(src_node.isPrimary()){
                        atomic_id = -1;
                    }else{
                        atomic_id = next_atomic_id;
                        next_atomic_id++;
                    }
                    auto uv4 = CityObjectIndex(primary_id, atomic_id).toUV();
                    auto uv4s = std::vector<TVec2f>(src_mesh_copy.getUV4().size(), uv4);
                    src_mesh_copy.setUV4(std::move(uv4s));
                    // マージします。
                    dst_mesh.merge(src_mesh_copy, false, true);

                    // CityObjectListを更新します。
                    const auto& src_city_obj_list = src_node.getMesh()->getCityObjectList();
                    // 入力は最小地物単位であるという前提なので、srcのCityObjectIndexは(0,0)です。
                    std::string gml_id = "gml_id_not_found";
                    src_city_obj_list.tryGetAtomicGmlID(CityObjectIndex(0, 0), gml_id);
                    auto& dst_city_obj_list = dst_mesh.getCityObjectList();
                    CityObjectIndex dst_city_obj_index;
                    dst_city_obj_index = CityObjectIndex(primary_id, atomic_id);
                    dst_city_obj_list.add(dst_city_obj_index, gml_id);
                }

                // 子ノードをキューに入れます。
                for(int i=0; i<src_node.getChildCount(); i++) {
                    queue.push(&src_node.getChildAt(i));
                }
            }

        }

        /// 最小地物単位のモデルを受け取り、それを地域単位に変換したモデルを返します。
        Model convertFromAtomicToArea(const Model& src) {
            auto dst_model = Model();
            const auto root_node_name = src.getRootNodeCount() == 1 ?
                    src.getRootNodeAt(0).getName() : "combined";
            auto dst_node_tmp = Node(root_node_name);
            dst_model.reserveRootNodes(src.getRootNodeCount());
            dst_model.addNode(std::move(dst_node_tmp));
            auto& dst_node = dst_model.getRootNodeAt(0);
            dst_node.setIsPrimary(true);
            dst_node.setMesh(std::make_unique<Mesh>());
            auto& dst_mesh = *dst_node.getMesh();

            auto queue = NodeBFSQueue();
            // ルートノードを探索キューに加えます。
            for(int i=0; i<src.getRootNodeCount(); i++) {
                auto& src_root_node = src.getRootNodeAt(i);
                queue.push(&src_root_node, &dst_node);
            }

            // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードを子を含めて結合し、primary_idをインクリメントします。
            long primary_id = 0;
            while(!queue.empty()) {
                const auto& src_node = std::get<0>(queue.pop());
                if(src_node->isPrimary()) {
                    // PrimaryなNodeが見つかったら、そのノードと子をマージします。
                    mergePrimaryNodeAndChildren(*src_node, dst_mesh, primary_id);
                    primary_id++;
                }else{
                    // PrimaryなNodeでなければ、Primaryに行き着くまで探索を続けます。
                    dst_node.reserveChild(src_node->getChildCount());
                    for(int i=0; i<src_node->getChildCount(); i++) {
                        queue.push(&src_node->getChildAt(i), &dst_node);
                    }
                }
            }
            return dst_model;
        }

        /// 最小地物単位のモデルを受け取り、それを主要地物単位に変換したモデルを返します。
        Model convertFromAtomicToPrimary(const Model& src_model){
            auto dst_model = Model();
            NodeBFSQueue queue;
            dst_model.reserveRootNodes(src_model.getRootNodeCount());
            // ルートノードを探索キューに加えると同時に、dst_modelに同じノードを準備します。
            for(int i=0; i<src_model.getRootNodeCount(); i++) {
                const auto& src_node = src_model.getRootNodeAt(i);
                dst_model.addNode(Node(src_node.getName()));
                auto& dst_node = dst_model.getRootNodeAt(i);
                queue.push(&src_node, &dst_node);
            }
            // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードの子を含めて結合します。そのprimary_idは0とします。
            while(!queue.empty()) {
                const auto& [src_node, dst_node] = queue.pop();
                if(src_node->isPrimary()) {
                    // Primaryなら、src_nodeとその子を結合したメッシュをdst_nodeに持たせます。
                    auto dst_mesh = std::make_unique<Mesh>();
                    mergePrimaryNodeAndChildren(*src_node, *dst_mesh, 0);
                    dst_node->setMesh(std::move(dst_mesh));
                }else{
                    dst_node->reserveChild(dst_node->getChildCount() + src_node->getChildCount());
                    // Primaryでないなら、子をキューに加えて探索を続けます。同じ子をdst_nodeに加えます。
                    for(int i=0; i<src_node->getChildCount(); i++) {
                        const auto& src_child = src_node->getChildAt(i);
                        auto& dst_child = dst_node->addChildNode(Node(src_child.getName()));
                        queue.push(&src_child, &dst_child);
                    }
                }
            }
            return dst_model;
        }

    }

    Model GranularityConverter::convert(const Model& src, GranularityConvertOption option) {
        // 組み合わせの数を減らすため、まず最小地物に変換してから望みの粒度に変換します。

        // 例：入力のNode構成が次のようだったとして、以下にその変化を例示します。
        // 入力： gml_node <- lod_node <- group_node

        auto atomic = convertToAtomic(src);

        // 例：上の行の実行後、次のようなNode構成になります。
        // gml_node <- lod_node <- group_node <- primary_node <- atomic_node

        movePrimaryNodesToParent(atomic);

        // 例：上の行の実行後、次のようなNode構成になります。
        // gml_node <- lod_node
        //                    ┣ group_node
        //                    ┗ primary_node <- atomic_node

        atomic.eraseEmptyNodes();

        // 例：上の行の実行後、次のようなNode構成になります。
        // gml_node <- lod_node <- primary_node <- atomic_node

        switch(option.granularity_){
            case MeshGranularity::PerAtomicFeatureObject:
                return atomic;
            case MeshGranularity::PerPrimaryFeatureObject:
                return convertFromAtomicToPrimary(atomic);
            case MeshGranularity::PerCityModelArea:
                return convertFromAtomicToArea(atomic);
            default:
                throw std::runtime_error("unknown argument");
        }
    }
}
