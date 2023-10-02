#include <plateau/granularity_convert/granularity_converter.h>
#include <queue>
#include <tuple>
#include <set>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;


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
        void moveNodesWithMeshToParent(Model& model) {
            // 幅優先探索の探索キューです。
            auto queue = NodeQueueOfIndexOfParent();

            // ルートノードをキューに加えます。
            for(int i=0; i<model.getRootNodeCount(); i++) {
                queue.push(nullptr, i);
            }

            while(!queue.empty()) {
                auto [parent_node, child_index] = queue.pop();

                auto current_node_tmp = NodeQueueOfIndexOfParent::getNodeFromParent(parent_node, child_index, model);

                // 下のif文に、子ノードを親に移動すべき条件を書きます。
                // ・ルートノードの場合は親がないので子を移動させません。
                // ・親にすでにメッシュがある場合は上書きさせたくないので移動させません。
                // ・現在のノードにメッシュがある場合は、親子関係を維持したいので子を動かしません。
                if(parent_node != nullptr && parent_node->getMesh() == nullptr && current_node_tmp->getMesh() == nullptr) {

                    // 子ノードのうち、メッシュが存在するものを親ノードに移動します。
                    // なお、ループの中では毎回current_nodeを取得する必要があります。
                    // current_nodeを使い回してしまうと、親にノードが増えたとき、vectorの再割り当てによってポインタが無効になることがあります。
                    std::set<int> child_indices_to_delete;
                    for(int i=0; i<NodeQueueOfIndexOfParent::getNodeFromParent(parent_node, child_index, model)->getChildCount(); i++) {
                        auto current_node = NodeQueueOfIndexOfParent::getNodeFromParent(parent_node, child_index, model);
                        auto child_node = &current_node->getChildAt(i);
                        if(child_node->getMesh() != nullptr) {

                            // この段階で、次のような階層構造になっているとします。
                            // parent <- current(meshなし) <- child(meshあり)

                            // ここで子を親に移動します。
                            parent_node->addChildNode(std::move(*child_node));

                            // この段階で次のようになります。
                            // parent
                            //      ┣ current(meshなし)  これはあとに実行されるeraseEmptyNodesで削除されます。
                            //      ┗ 旧child(meshあり)

                            // currentの子のi番目は、移動したので削除すべきと覚えておきます。
                            child_indices_to_delete.insert(i);

                            // 親に移動した子について、さらにその子に移動すべきノードがあるかもしれません。
                            // 例： parent
                            //          ┣ current(meshなし) (あとで削除)
                            //          ┗ 旧child(meshあり) <- meshなしノード (不要) <- meshありノード(必要)
                            // この末尾のmeshありノードを上の階層に移動したいです。
                            // したがって、移動したノードも探索キューに加えます。
                            //
                            // 補足：
                            // どのような時に上の例のような階層構造になるかというと、最小地物単位を最小地物単位に変換したとき、下のようになってこのメソッドに入力されます。
                            //
                            // lod <- primary(meshなし) <- primary(meshあり) <- atomic(meshなし) <- atomic(meshあり)
                            //
                            // これを処理すると、初めてこの行を実行する段階では次のようになっています。
                            //
                            // lod
                            //  ┣ primary(meshなし)
                            //  ┗ primary(meshあり) <- atomic(meshなし,不要) <- atomic(meshあり)
                            //
                            // このケースでのmeshなしノードは不要なので階層構造からスキップしたいです。
                            queue.push(parent_node, (int)parent_node->getChildCount() - 1);
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
                // 現在ノードの子をキューに加えます。
                for(int i=0; i<current_node->getChildCount(); i++) {
                    queue.push(current_node, i);
                }
            }


        }

        /// MeshのうちCityObjectIndexが引数idに一致する箇所のみを取り出したMeshを生成して返します。
        Mesh filterByCityObjIndex(const Mesh& src, CityObjectIndex filter_id, const int uv4_atomic_index) {
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
                    dst_uv4.push_back({0, (float)uv4_atomic_index});
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
        Model convertToAtomic(Model& src) {
            Model dst_model = Model();

            // 探索キューであり、変換元のノードを指すキューと、それに対応する変換後のノードを指すキューです。
            auto src_queue = NodeQueueOfIndexOfParent();
            auto dst_queue = NodeQueueOfIndexOfParent();

            dst_model.reserveRootNodes(src.getRootNodeCount());

            // ルートノードをdstに作って探索キューに入れます。
            for(int i=0; i<src.getRootNodeCount(); i++) {
                auto& src_node = src.getRootNodeAt(i);
                auto dst_node = Node(src_node.getName());
                dst_model.addNode(std::move(dst_node));
                src_queue.push(nullptr, i);
                dst_queue.push(nullptr, i);
            }

            // 幅優先探索の順番で変換します。
            while(!src_queue.empty()) {
                NodeQueueOfIndexOfParent src_next_parent_nodes;
                NodeQueueOfIndexOfParent dst_next_parent_nodes;

                while(!src_queue.empty()) {
                    const auto [src_parent_node, src_child_index] = src_queue.pop();
                    auto [dst_parent_node, dst_child_index] = dst_queue.pop();

                    auto src_mesh_tmp = NodeQueueOfIndexOfParent::getNodeFromParent(src_parent_node, src_child_index, src)->getMesh();
                    if(src_mesh_tmp != nullptr) {
                        const auto src_node = NodeQueueOfIndexOfParent::getNodeFromParent(src_parent_node, src_child_index, src);

                        // どのインデックス(uv4)が含まれるかを列挙します。
                        const auto src_mesh = src_node->getMesh();
                        std::set<CityObjectIndex> indices_in_mesh;
                        std::set<int> primary_indices_in_mesh;
                        for(const auto& uv4 : src_mesh->getUV4()) {
                            auto id = CityObjectIndex::fromUV(uv4);
                            indices_in_mesh.insert(id);
                            primary_indices_in_mesh.insert(id.primary_index);
                        }

                        const auto& src_city_obj_list = src_mesh->getCityObjectList();

                        // PrimaryIndexごとの処理
                        for(auto primary_id : primary_indices_in_mesh) {
                            bool is_parent_primary = dst_parent_node != nullptr && dst_parent_node->isPrimary();


                            Node* primary_node;
                            if(is_parent_primary) {
                                primary_node = dst_parent_node;
                            }else{
                                // 親がPrimary Nodeでない場合は、Primary Nodeを作ります。
                                std::string primary_gml_id = "gml_id_not_found";
                                src_city_obj_list.tryGetPrimaryGmlID(primary_id, primary_gml_id);
                                // ここでノードを追加します。
                                primary_node = dst_parent_node == nullptr ?
                                                     &dst_model.addNode(Node(primary_gml_id)) : // ルートに追加
                                                     &dst_parent_node->addChildNode(Node(primary_gml_id)); // ノードに追加
                                primary_node->setIsPrimary(true);
                                dst_child_index++; // TODO
                                auto primary_mesh = filterByCityObjIndex(*src_mesh, CityObjectIndex(primary_id, -1), -1);
                                if(primary_mesh.hasVertices()){
                                    primary_mesh.setCityObjectList({{{{0, -1}, primary_gml_id}}});
                                    primary_node->setMesh(std::make_unique<Mesh>(primary_mesh));
                                }
                            }




                            // PrimaryIndex相当のノードの子に、AtomicIndex相当のノードを作ります。
                            for(const auto& id : indices_in_mesh) {
                                if(id.primary_index != primary_id) continue;
                                if(id.atomic_index == CityObjectIndex::invalidIndex()) continue;
                                std::string atomic_gml_id = "gml_id_not_found";
                                src_city_obj_list.tryGetAtomicGmlID(id, atomic_gml_id);
                                // ここでノードを追加します。
                                auto& atomic_node = primary_node == nullptr ?
                                        dst_model.addNode(Node(atomic_gml_id)) :
                                        primary_node->addChildNode(Node(atomic_gml_id));

                                auto atomic_mesh = filterByCityObjIndex(*src_mesh, id, 0);
                                if(atomic_mesh.hasVertices()){
                                    atomic_mesh.setCityObjectList({{{{0, 0}, atomic_gml_id}}});
                                    atomic_node.setMesh(std::make_unique<Mesh>(atomic_mesh));
                                }
                            } // end for(atomic)
                        } // end for(primary)
                        // end if(メッシュがあるとき)
                    }else{ // メッシュがないとき
                        // メッシュのないノードをdstに追加します
//                        auto dst_node = Node(NodeQueueOfIndexOfParent::getNodeFromParent(src_parent_node, src_child_index, src)->getName());
//                        if(dst_parent_node == nullptr){
//                            dst_model.addNode(std::move(dst_node));
//                        }else{
//                            dst_parent_node->addChildNode(std::move(dst_node));
//                        }

                    }
                    // 次に探索すべきノードの親
                    src_next_parent_nodes.push(src_parent_node, src_child_index);
                    dst_next_parent_nodes.push(dst_parent_node, dst_child_index);




                }// end while 2段目

                // 子をキューに積みます。
                while(!src_next_parent_nodes.empty()) {
                    auto [src_parent_parent_node, src_child_index] = src_next_parent_nodes.pop();
                    auto [dst_parent_parent_node, dst_child_index] = dst_next_parent_nodes.pop();
//                    auto dst_child_index = dst_child_index_old + new_node_count_of_dst_parent;
                    auto src_parent_node = NodeQueueOfIndexOfParent::getNodeFromParent(src_parent_parent_node, src_child_index, src);
                    auto dst_parent_node = NodeQueueOfIndexOfParent::getNodeFromParent(dst_parent_parent_node, dst_child_index, dst_model);
                    for(int i=0; i<src_parent_node->getChildCount(); i++) {
                        auto& src_child = src_parent_node->getChildAt(i);
//                        if(dst_parent_node->getChildCount() > i && dst_parent_node->getChildAt(i).isPrimary()){
                        if(dst_parent_node->getMesh() != nullptr) {
                            continue;
                        }
                        src_queue.push(src_parent_node, i);
                        dst_parent_node->addChildNode(Node(src_child.getName()));
                        dst_queue.push(dst_parent_node, (int)dst_parent_node->getChildCount() - 1);
                    }
                }
            } // end while 1段目

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

                    // 入力メッシュのgml_idを取得します。
                    // 入力は最小地物単位であるという前提なので、srcのCityObjectIndexは(0,0)か(0,-1)のどちらかです。
                    const static std::string default_gml_id = "gml_id_not_found";
                    std::string gml_id = default_gml_id;
                    src_city_obj_list.tryGetAtomicGmlID(CityObjectIndex(0, 0), gml_id);
                    if(gml_id == default_gml_id) { // 見つからないとき
                        src_city_obj_list.tryGetAtomicGmlID(CityObjectIndex(0, -1), gml_id);
                    }

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
            auto src_queue = NodeQueueOfIndexOfParent();
            auto dst_queue = NodeQueueOfIndexOfParent();

            // ルートノードを探索キューに加えます。
            for(int i=0; i<src.getRootNodeCount(); i++) {
                src_queue.push(nullptr, i);
            }

            // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードを子を含めて結合し、primary_idをインクリメントします。
            long primary_id = 0;
            while(!src_queue.empty()) {
                const auto [src_parent_node, src_parent_index] = src_queue.pop();
                const auto& src_node = NodeQueueOfIndexOfParent::getNodeFromParent(src_parent_node, src_parent_index, src);
                if(src_node->isPrimary()) {
                    // PrimaryなNodeが見つかったら、そのノードと子をマージします。
                    mergePrimaryNodeAndChildren(*src_node, dst_mesh, primary_id);
                    primary_id++;
                }else{
                    // PrimaryなNodeでなければ、Primaryに行き着くまで探索を続けます。
                    dst_node.reserveChild(src_node->getChildCount());
                    for(int i=0; i<src_node->getChildCount(); i++) {
                        src_queue.push(src_node, i);
                    }
                }
            }
            return dst_model;
        }

        /// 最小地物単位のモデルを受け取り、それを主要地物単位に変換したモデルを返します。
        Model convertFromAtomicToPrimary(Model& src_model){
            auto dst_model = Model();
            NodeQueueOfIndexOfParent src_queue;
            NodeQueueOfIndexOfParent dst_queue;
            dst_model.reserveRootNodes(src_model.getRootNodeCount());
            // ルートノードを探索キューに加えると同時に、dst_modelに同じノードを準備します。
            for(int i=0; i<src_model.getRootNodeCount(); i++) {
                const auto& src_node = src_model.getRootNodeAt(i);
                dst_model.addNode(Node(src_node.getName()));
                auto& dst_node = dst_model.getRootNodeAt(i);
                src_queue.push(nullptr, i);
                dst_queue.push(nullptr, i);
            }
            // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードの子を含めて結合します。そのprimary_idは0とします。
            while(!src_queue.empty()) {
                const auto [src_parent_node, src_parent_index] = src_queue.pop();
                const auto [dst_parent_node, dst_parent_index] = dst_queue.pop();
                auto src_node = NodeQueueOfIndexOfParent::getNodeFromParent(src_parent_node, src_parent_index, src_model);
                auto dst_node = NodeQueueOfIndexOfParent::getNodeFromParent(dst_parent_node, dst_parent_index, dst_model);
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
                        src_queue.push(src_node, i);
                        dst_queue.push(dst_node, dst_node->getChildCount() - 1);
                    }
                }
            }
            return dst_model;
        }

    }

    Model GranularityConverter::convert(Model& src, GranularityConvertOption option) {
        // 組み合わせの数を減らすため、まず最小地物に変換してから望みの粒度に変換します。

        // 例：入力のNode構成が次のようだったとして、以下にその変化を例示します。
        // 入力： gml_node <- lod_node <- group_node

        auto atomic = convertToAtomic(src);

        // 例：上の行の実行後、次のようなNode構成になります。
        // gml_node <- lod_node <- group_node <- primary_node <- atomic_node

//        moveNodesWithMeshToParent(atomic);

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
