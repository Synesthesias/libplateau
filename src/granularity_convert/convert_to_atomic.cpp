#include <plateau/granularity_convert/convert_to_atomic.h>
#include <plateau/granularity_convert/node_queue.h>
#include <set>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    namespace {
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
                if (src_end < src_start) continue;

                // 新しいSubMeshです。
                auto dst_start = indices_id_transform.at(src_start);
                auto dst_end = indices_id_transform.at(src_end);
                auto dst_sub_mesh = src_sub_mesh;
                dst_sub_mesh.setStartIndex(dst_start);
                dst_sub_mesh.setEndIndex(dst_end);
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

        class CityObjIndexSet {
        public:
            void insert(const CityObjectIndex& city_obj_index) {
                indices_.insert(city_obj_index);
            }

            size_t sizeOfPrimary() {
                size_t ret = 0;
                for(const auto& index : indices_) {
                    if(index.atomic_index == CityObjectIndex::invalidIndex()) ret++;
                }
                return ret;
            }

            size_t sizeOfAtomic() {
                return indices_.size() - sizeOfPrimary();
            }

            const std::set<CityObjectIndex>& get() {
                return indices_;
            }
        private:
            std::set<CityObjectIndex> indices_;
        };
    }

    Model ConvertToAtomic::convert(const Model* src) const {
        Model dst_model = Model();

        // 探索キュー
        auto queue = NodeQueue();

        dst_model.reserveRootNodes(src->getRootNodeCount());

        // ルートノードを探索キューに入れます。
        for (int i = 0; i < src->getRootNodeCount(); i++) {
            queue.push(NodePath({i}));
        }

        // 幅優先探索の順番で変換します。
        while (!queue.empty()) {
            auto node_path = queue.pop();

            // 子をキューに追加
            {
                auto src_node = node_path.toNode(src);
                for (int i = 0; i < src_node->getChildCount(); i++) {
                    queue.push(node_path.plus(i));
                }
            }

            auto src_mesh_tmp = node_path.toNode(src)->getMesh();
            if (src_mesh_tmp != nullptr && src_mesh_tmp->hasVertices()) {
                const auto src_node = node_path.toNode(src);

                // どのインデックス(uv4)が含まれるかを列挙します。
                const auto src_mesh = src_node->getMesh();
                CityObjIndexSet indices_in_mesh;
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
                    // 自身のメッシュUVが(0,-1)のみで、親にメッシュがないとき、親を主要地物とみなします。
                    // 特別ケースB:
                    // 入力が主要地物単位で、LOD（メッシュなし） <- Primary(Atomicメッシュのみ)の構成で、現在がPrimaryのケース。
                    // 自身を主要地物とみなします。
                    auto dst_parent_node = node_path.parent().toNode(&dst_model);
                    if (dst_parent_node != nullptr && primary_node_path.empty() && node_path.toNode(src)->getChildCount() == 0) {
                        bool has_only_0_0_mesh = indices_in_mesh.sizeOfPrimary() == 0 &&
                                                    indices_in_mesh.sizeOfAtomic() == 1;
                        bool has_only_0_over_minus1_mesh = indices_in_mesh.sizeOfPrimary() == 0 &&
                                                           indices_in_mesh.sizeOfAtomic() > 0;

                        auto* parent = node_path.parent().parent().toNode(&dst_model);
                        bool parent_has_no_mesh =
                                parent == nullptr || parent->getMesh() == nullptr || (!parent->getMesh()->hasVertices());

                        bool is_special_case_a =
                                parent_has_no_mesh && has_only_0_0_mesh;
                        bool is_special_case_b = (!is_special_case_a) &&
                                parent_has_no_mesh && has_only_0_over_minus1_mesh;
                        if (is_special_case_a) {
                            dst_parent_node->setIsPrimary(true);
                            primary_node_path = node_path.parent();
                        } else if (is_special_case_b) {
                            auto& primary = node_path.parent().toNode(&dst_model)->addChildNode(
                                    Node(src_node->getName()));
                            primary.setIsPrimary(true);
                            primary_node_path = node_path.parent().plus(
                                    node_path.parent().toNode(&dst_model)->getChildCount() - 1);
                        }
                    } // 特別ケース ここまで

                    Node* new_primary_node = primary_node_path.toNode(&dst_model);
                    bool primary_node_created = false;
                    if (primary_node_path.empty() || primary_node_path == node_path) {
                        // 親にPrimary Nodeがなく、メッシュがある場合は、Primary Nodeを作ります。
                        auto primary_mesh = filterByCityObjIndex(*src_mesh, CityObjectIndex(primary_id, -1), -1);
                        if (primary_mesh.hasVertices()) {
                            const static std::string default_gml_id = "gml_id_not_found";
                            std::string primary_gml_id = default_gml_id;
                            src_city_obj_list.tryGetPrimaryGmlID(primary_id, primary_gml_id);
                            const std::string primary_node_name = primary_gml_id == default_gml_id ?
                                                                  node_path.toNode(src)->getName() : primary_gml_id;
                            // ここでノードを追加します。
                            auto dst_parent = node_path.parent().toNode(&dst_model);
                            new_primary_node = dst_parent == nullptr ?
                                               &dst_model.addNode(Node(primary_node_name)) : // ルートに追加
                                               &dst_parent->addChildNode(Node(primary_node_name)); // ノードに追加
                            new_primary_node->setIsPrimary(true);
                            primary_mesh.setCityObjectList({{{{0, -1}, primary_gml_id}}});
                            new_primary_node->setMesh(std::make_unique<Mesh>(primary_mesh));
                            primary_node_created = true;
                        }
                    }

                    // PrimaryIndex相当のノードの子に、AtomicIndex相当のノードを作ります。
                    for (const auto& id : indices_in_mesh.get()) {
                        if (id.primary_index != primary_id) continue;

                        // 制作済みのものはスキップ
                        if (id.atomic_index == CityObjectIndex::invalidIndex() && primary_node_created) continue;

                        const static std::string default_gml_id = "gml_id_not_found";
                        std::string atomic_gml_id = default_gml_id;
                        src_city_obj_list.tryGetAtomicGmlID(id, atomic_gml_id);
                        std::string node_name = atomic_gml_id == default_gml_id ? src_node->getName() : atomic_gml_id;

                        auto atomic_mesh = filterByCityObjIndex(*src_mesh, id, 0);
                        if (atomic_mesh.hasVertices() || atomic_mesh.getCityObjectList().size() > 0) {
                            // ここでノードを追加します。
                            auto& atomic_node = new_primary_node == nullptr ?
                                    dst_model.addNode(Node(node_name)) :
                                    new_primary_node->addChildNode(Node(node_name));
                            // 最小地物のGML IDを記録します。
                            atomic_mesh.setCityObjectList({{{{0, 0}, atomic_gml_id}}});
                            // 親となる主要地物のGML IDを記録します。実際は最小地物のみのMeshであっても、対応する主要地物を記憶しておきます。
                            std::string primary_gml_id = default_gml_id;
                            src_city_obj_list.tryGetPrimaryGmlID(id.primary_index, primary_gml_id);
                            if(primary_gml_id == default_gml_id){
//                                auto primary_node = primary_node_path.toNode(&dst_model);
//                                if(primary_node != nullptr){
//                                    primary_gml_id = primary_node->getName();
//                                }
                            }else{
                                // 最小地物を見て、はじめて親の主要地物のGML IDが判別できるケースがあります。
                                auto primary_node = primary_node_path.toNode(&dst_model);
                                if(primary_node != nullptr) {
                                    primary_node->setName(primary_gml_id);
                                    auto mesh = primary_node->getMesh();
                                    if(mesh == nullptr) {
                                        auto mesh_tmp = std::make_unique<Mesh>();
                                        primary_node->setMesh(std::move(mesh_tmp));
                                        mesh = primary_node->getMesh();
                                    }
                                    auto& primary_city_obj_list = mesh->getCityObjectList();
                                    if(!primary_city_obj_list.containsCityObjectIndex(CityObjectIndex(0, -1))) {
                                        primary_city_obj_list.add(CityObjectIndex(0,-1), primary_gml_id);
                                    }
                                }
                            }
                            const auto primary_id_of_parent_of_atomic = CityObjectIndex(0, CityObjectIndex::invalidIndex());
                            atomic_mesh.getCityObjectList().add(primary_id_of_parent_of_atomic, primary_gml_id);

                            atomic_node.setMesh(std::make_unique<Mesh>(atomic_mesh));
                        }
                    }
                }
                // end if(メッシュがあるとき)
            } else { // メッシュがないとき
                // メッシュのないノードをdstに追加します
                const auto src_node = node_path.toNode(src);
                auto& dst_node = node_path.parent().addChildNode(Node(src_node->getName()), &dst_model);
                dst_node.setIsPrimary(src_node->isPrimary());
            }
        }

        dst_model.eraseEmptyNodes();
        return dst_model;
    }
}
