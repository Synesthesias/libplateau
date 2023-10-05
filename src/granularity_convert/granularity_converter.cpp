#include <plateau/granularity_convert/granularity_converter.h>
#include <queue>
#include <tuple>
#include <set>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;


    namespace {

        /// ルートノードから何番目の子をたどっていけばノードに行き着くかをvector<int>で表現したノードパスです。
        class NodePath {
        public:
            NodePath(std::vector<int> positions) : positions_(std::move(positions)) {};

            Node* toNode(Model* model) const {
                if (positions_.empty()) return nullptr;
                auto node = &model->getRootNodeAt(positions_.at(0));
                for (int i = 1; i < positions_.size(); i++) {
                    node = &node->getChildAt(positions_.at(i));
                }
                return node;
            };

            NodePath parent() const {
                auto new_pos = std::vector(positions_.begin(), positions_.end() - 1);
                return {new_pos};
            }

            NodePath plus(int next_index) const {
                auto new_pos = std::vector(positions_);
                new_pos.push_back(next_index);
                return new_pos;
            }

            NodePath next() const {
                auto ret = NodePath(positions_);
                ret.positions_.at(ret.positions_.size() - 1) = ret.positions_.at(ret.positions_.size() - 1) + 1;
                return ret;
            }

            bool empty() const {
                return positions_.empty();
            }

            Node& addChildNode(Node&& node, Model* model) const {
                if (positions_.empty()) {
                    return model->addNode(std::move(node));
                }
                return toNode(model)->addChildNode(std::move(node));
            }

            // パス中にプライマリノードがあればそのパスを返し、なければ空のNodePathを返します。
            // パス中に複数ある場合は、最後のものを返します。
            NodePath searchLastPrimaryNodeInPath(Model* model) {
                int found_path_pos = -1;
                if(positions_.empty()) return NodePath({});
                if(model->getRootNodeCount() >= positions_.at(0)) return NodePath({});
                auto node = &model->getRootNodeAt(positions_.at(0));
                if(node->isPrimary()) found_path_pos = 0;
                for(int i=1; i<positions_.size(); i++) {
                    if(positions_.at(i) >= node->getChildCount()) break;
                    node = &node->getChildAt(positions_.at(i));
                    if(node->isPrimary()) found_path_pos = i;
                }
                if(found_path_pos < 0) return NodePath({});
                auto ret_path = NodePath(std::vector<int>(positions_.begin(), positions_.begin() + found_path_pos + 1));
                return ret_path;
            }

        private:
            std::vector<int> positions_;
        };

        class NodeQueue {
        public:
            void push(NodePath pos) { queue_.push(std::move(pos)); };

            NodePath pop() {
                auto ret = queue_.front();
                queue_.pop();
                return ret;
            };

            bool empty() {
                return queue_.empty();
            }

        private:
            std::queue<NodePath> queue_;
        };


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
            for (std::size_t i = 0; i < vertex_count; i++) {
                auto src_id = CityObjectIndex::fromUV(src_uv4.at(i));
                if (src_id == filter_id) {
                    vert_id_transform.push_back((long) current_vert_id);
                    dst_vertices.push_back(src_vertices.at(i));
                    dst_uv1.push_back(src_uv1.at(i));
                    dst_uv4.emplace_back(0, (float) uv4_atomic_index);
                    ++current_vert_id;
                } else {
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
            for (auto src_index: src_indices) {
                const auto next_id = vert_id_transform.at(src_index); // 削除頂点を詰めたあとの新たな頂点番号
                if (next_id < 0) {
                    indices_id_transform.push_back(-1);
                    continue;
                }
                dst_indices.push_back(next_id);
                indices_id_transform.push_back((long) dst_indices.size() - 1);
            }

            // SubMeshについて、元から削除部分を除いたvector<SubMesh>を生成します。
            auto dst_sub_meshes = std::vector<SubMesh>();
            for (const auto& src_sub_mesh: src_sub_meshes) {
                auto src_start = src_sub_mesh.getStartIndex();
                auto src_end = src_sub_mesh.getEndIndex();

                // src_startが削除されなかった部分にヒットするまで右に移動します。
                while (indices_id_transform.at(src_start) < 0) {
                    src_start++;
                    if (src_start > src_end) break; // src_startからsrc_endまでの範囲がすべて削除されていたケース
                }
                if (src_start > src_end) continue;

                // src_endが削除されなかった部分にヒットするまで左に移動します。
                while (indices_id_transform.at(src_end) < 0) {
                    src_end--;
                    if (src_end < src_start) break; // src_startからsrc_endまでの範囲がすべて削除されていたケース
                }
                if (src_end < src_start) break;

                // 新しいSubMeshです。
                auto dst_start = indices_id_transform.at(src_start);
                auto dst_end = indices_id_transform.at(src_end);
                auto dst_sub_mesh = src_sub_mesh;
                dst_sub_mesh.setStartIndex((int) dst_start);
                dst_sub_mesh.setEndIndex((int) dst_end);
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

            // 探索キュー
            auto queue = NodeQueue();

            dst_model.reserveRootNodes(src.getRootNodeCount());

            // ルートノードを探索キューに入れます。
            for (int i = 0; i < src.getRootNodeCount(); i++) {
                queue.push(NodePath({i}));
            }

            // 幅優先探索の順番で変換します。
            while (!queue.empty()) {
                auto node_path = queue.pop();

                // 子をキューに追加
                {
                    auto src_node = node_path.toNode(&src);
                    for (int i = 0; i < src_node->getChildCount(); i++) {
                        queue.push(node_path.plus(i));
                    }
                }

                auto src_mesh_tmp = node_path.toNode(&src)->getMesh();
                if (src_mesh_tmp != nullptr) {
                    const auto src_node = node_path.toNode(&src);

                    // どのインデックス(uv4)が含まれるかを列挙します。
                    const auto src_mesh = src_node->getMesh();
                    std::set<CityObjectIndex> indices_in_mesh;
                    std::set<int> primary_indices_in_mesh;
                    for (const auto& uv4: src_mesh->getUV4()) {
                        auto id = CityObjectIndex::fromUV(uv4);
                        indices_in_mesh.insert(id);
                        primary_indices_in_mesh.insert(id.primary_index);
                    }

                    const auto& src_city_obj_list = src_mesh->getCityObjectList();




                    // PrimaryIndexごとの処理
                    for (auto primary_id: primary_indices_in_mesh) {
                        NodePath primary_node_path = node_path.parent().searchLastPrimaryNodeInPath(&dst_model);
                        // 特別ケースA:
                        // 入力が最小地物単位で、Primary（メッシュなし）<- Atomic（メッシュあり）の構成で、現在がatomicのケース。
                        // 自身のメッシュが最小地物のみで、親にメッシュがないとき、親を主要地物とみなします。
                        // 特別ケースB:
                        // 入力が主要地物単位で、LOD（メッシュなし） <- Primary(Atomicメッシュのみ)の構成で、現在がPrimaryのケース。
                        // 特別ケースAとBを本質的に区別する方法が見当たらないので、メッシュ中のatomicが1つならA, 2つ以上ならBという仮説で実装します。
                        // 自身を主要地物とみなします。
                        auto dst_parent_node = node_path.parent().toNode(&dst_model);
                        if(dst_parent_node != nullptr && primary_node_path.empty()) {
                            bool has_only_atomic_mesh = src_city_obj_list.getAllPrimaryIndices().empty() &&
                                                        (!src_city_obj_list.getAllAtomicIndices().empty());
                            bool is_special_case =
                                    has_only_atomic_mesh &&
                                    node_path.parent().parent().toNode(&dst_model)->getMesh() == nullptr;

                            bool is_special_case_a = is_special_case && (src_city_obj_list.getAllAtomicIndices().size() == 1);
                            bool is_special_case_b = is_special_case && (src_city_obj_list.getAllAtomicIndices().size() >= 1);
                            if(is_special_case_a) {
                                dst_parent_node->setIsPrimary(true);
                                primary_node_path = node_path.parent();
                            }else if(is_special_case_b) {
                                auto& primary = node_path.parent().toNode(&dst_model)->addChildNode(Node(src_node->getName()));
                                primary.setIsPrimary(true);
                                primary_node_path = node_path;
                            }
                        }

                        Node* new_primary_node = primary_node_path.toNode(&dst_model);
                        if (primary_node_path.empty() || primary_node_path.toNode(&src) == node_path.toNode(&src)) {
                            // 親にPrimary Nodeがない場合は、Primary Nodeを作ります。
                            std::string primary_gml_id = "gml_id_not_found";
                            src_city_obj_list.tryGetPrimaryGmlID(primary_id, primary_gml_id);
                            // ここでノードを追加します。
                            auto dst_parent = node_path.parent().toNode(&dst_model);
                            new_primary_node = dst_parent == nullptr ?
                                           &dst_model.addNode(Node(primary_gml_id)) : // ルートに追加
                                           &dst_parent->addChildNode(Node(primary_gml_id)); // ノードに追加
                            new_primary_node->setIsPrimary(true);
                            auto primary_mesh = filterByCityObjIndex(*src_mesh, CityObjectIndex(primary_id, -1), -1);
                            if (primary_mesh.hasVertices()) {
                                primary_mesh.setCityObjectList({{{{0, -1}, primary_gml_id}}});
                                new_primary_node->setMesh(std::make_unique<Mesh>(primary_mesh));
                            }
                        }

                        // PrimaryIndex相当のノードの子に、AtomicIndex相当のノードを作ります。
                        for (const auto& id: indices_in_mesh) {
                            if (id.primary_index != primary_id) continue;
                            if (id.atomic_index == CityObjectIndex::invalidIndex()) continue;

                            std::string atomic_gml_id = "gml_id_not_found";
                            src_city_obj_list.tryGetAtomicGmlID(id, atomic_gml_id);
                            // ここでノードを追加します。
                            auto& atomic_node = new_primary_node->addChildNode(Node(atomic_gml_id));

                            auto atomic_mesh = filterByCityObjIndex(*src_mesh, id, 0);
                            if (atomic_mesh.hasVertices()) {
                                atomic_mesh.setCityObjectList({{{{0, 0}, atomic_gml_id}}});
                                atomic_node.setMesh(std::make_unique<Mesh>(atomic_mesh));
                            }
                        }
                    }
                    // end if(メッシュがあるとき)
                } else { // メッシュがないとき
                    // メッシュのないノードをdstに追加します
                    auto src_node = node_path.toNode(&src);
                    auto& dst_node = node_path.parent().addChildNode(Node(src_node->getName()), &dst_model);
                    dst_node.setIsPrimary(src_node->isPrimary());
                }
            }
            return dst_model;
        }

        /// 主要地物のノードとその子ノードを結合したものを、引数dst_meshに格納します。
        void mergePrimaryNodeAndChildren(const Node& src_node_arg, Mesh& dst_mesh, int primary_id) {
            std::queue<const Node*> queue;
            queue.push(&src_node_arg);
            long next_atomic_id = 0;

            while (!queue.empty()) {
                const auto& src_node = *queue.front();
                queue.pop();

                // メッシュをマージします。
                if (src_node.getMesh() != nullptr) {
                    // 元メッシュをコピーします。重いので注意してください。
                    auto src_mesh_copy = Mesh(*src_node.getMesh());
                    // UV4を置き換えます。
                    int atomic_id;
                    if (src_node.isPrimary()) {
                        atomic_id = -1;
                    } else {
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
                    if (gml_id == default_gml_id) { // (0,0)でないなら(0,-1)のはず
                        src_city_obj_list.tryGetAtomicGmlID(CityObjectIndex(0, -1), gml_id);
                    }

                    auto& dst_city_obj_list = dst_mesh.getCityObjectList();
                    CityObjectIndex dst_city_obj_index;
                    dst_city_obj_index = CityObjectIndex(primary_id, atomic_id);
                    dst_city_obj_list.add(dst_city_obj_index, gml_id);
                }

                // 子ノードをキューに入れます。
                for (int i = 0; i < src_node.getChildCount(); i++) {
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
                    mergePrimaryNodeAndChildren(*src_node, dst_mesh, primary_id);
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

        /// 最小地物単位のモデルを受け取り、それを主要地物単位に変換したモデルを返します。
        Model convertFromAtomicToPrimary(Model& src_model) {
            auto dst_model = Model();
            auto queue = NodeQueue();
            dst_model.reserveRootNodes(src_model.getRootNodeCount());
            // ルートノードを探索キューに加えると同時に、dst_modelに同じノードを準備します。
            for (int i = 0; i < src_model.getRootNodeCount(); i++) {
                const auto& src_node = src_model.getRootNodeAt(i);
                dst_model.addNode(Node(src_node.getName()));
                auto& dst_node = dst_model.getRootNodeAt(i);
                queue.push(NodePath({i}));
            }
            // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードの子を含めて結合します。そのprimary_idは0とします。
            while (!queue.empty()) {
                const auto node_path = queue.pop();
                auto src_node = node_path.toNode(&src_model);
                auto dst_node = node_path.toNode(&dst_model);
                if (src_node->isPrimary()) {
                    // Primaryなら、src_nodeとその子を結合したメッシュをdst_nodeに持たせます。
                    auto dst_mesh = std::make_unique<Mesh>();
                    mergePrimaryNodeAndChildren(*src_node, *dst_mesh, 0);
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

    Model GranularityConverter::convert(Model& src, GranularityConvertOption option) {
        // 組み合わせの数を減らすため、まず最小地物に変換してから望みの粒度に変換します。

        // 例：入力のNode構成が次のようだったとして、以下にその変化を例示します。
        // 入力： gml_node <- lod_node <- group_node

        auto atomic = convertToAtomic(src);

        // 例：上の行の実行後、次のようなNode構成になります。
        // gml_node <- lod_node <- primary_node <- atomic_node

        atomic.eraseEmptyNodes();

        switch (option.granularity_) {
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
