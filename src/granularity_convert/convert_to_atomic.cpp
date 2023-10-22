#include <plateau/granularity_convert/convert_to_atomic.h>
#include <plateau/granularity_convert/node_queue.h>
#include <plateau/granularity_convert/filter_by_city_obj_index.h>
#include <set>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    Model ConvertToAtomic::convert(const Model* src) const {
        Model dst_model = Model();

        // 探索キュー
        auto queue = NodeQueue();

        dst_model.reserveRootNodes(src->getRootNodeCount());

        // ルートノードを探索キューに入れます。
        queue.pushRoot(src);

        // 幅優先探索の順番で変換します。
        while (!queue.empty()) {
            auto node_path = queue.pop();
            queue.pushChildren(node_path, src);

            auto src_mesh_tmp = node_path.toNode(src)->getMesh();
            if (src_mesh_tmp != nullptr && src_mesh_tmp->hasVertices()) {
                const auto src_node = node_path.toNode(src);

                // どのインデックス(uv4)が含まれるかを列挙します。
                const auto src_mesh = src_node->getMesh();
                CityObjectIndexSet indices_in_mesh;
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
                        auto primary_mesh = FilterByCityObjIndex().filter(*src_mesh, CityObjectIndex(primary_id, -1), -1);
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

                        auto atomic_mesh = FilterByCityObjIndex().filter(*src_mesh, id, 0);
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
