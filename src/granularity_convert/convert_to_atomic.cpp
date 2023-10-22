#include <plateau/granularity_convert/convert_to_atomic.h>
#include <plateau/granularity_convert/node_queue.h>
#include <plateau/granularity_convert/filter_by_city_obj_index.h>
#include <set>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    namespace {
        /// 引数のnode_pathの親（再帰的な親を含む）からPrimaryNodeとするべきものを探します。
        NodePath calcPrimaryNodePathInParent(
                const NodePath& node_path, const Model& src_model, Model& dst_model,
                const CityObjectIndexSet& indices_in_mesh) {
            // 特別ケースA:
            // 入力が最小地物単位で、Primary（メッシュなし）<- Atomic（メッシュあり）の構成で、現在がatomicのケース。
            // 自身のメッシュUVが(0,-1)のみで、親にメッシュがないとき、親を主要地物とみなします。
            // 特別ケースB:
            // 入力が主要地物単位で、LOD（メッシュなし） <- Primary(Atomicメッシュのみ)の構成で、現在がPrimaryのケース。
            // 自身を主要地物とみなします。
            auto dst_parent_node = node_path.parent().toNode(&dst_model);

            // TODO ここに違和感
            NodePath primary_node_path = node_path.parent().searchLastPrimaryNodeInPath(&dst_model);

            if (dst_parent_node != nullptr && primary_node_path.empty() &&
                node_path.toNode(&src_model)->getChildCount() == 0) {
                bool has_only_0_0_mesh = indices_in_mesh.sizeOfPrimary() == 0 &&
                                         indices_in_mesh.sizeOfAtomic() == 1;
                bool has_only_0_over_minus1_mesh = indices_in_mesh.sizeOfPrimary() == 0 &&
                                                   indices_in_mesh.sizeOfAtomic() > 0;

                auto* parent = node_path.parent().parent().toNode(&dst_model);
                bool parent_has_no_mesh = parent == nullptr || (!parent->hasVertices());

                bool is_special_case_a =
                        parent_has_no_mesh && has_only_0_0_mesh;
                bool is_special_case_b = (!is_special_case_a) &&
                                         parent_has_no_mesh && has_only_0_over_minus1_mesh;
                if (is_special_case_a) {
                    dst_parent_node->setIsPrimary(true);
                    primary_node_path = node_path.parent();
                } else if (is_special_case_b) {
                    auto& primary = node_path.parent().toNode(&dst_model)->addChildNode(
                            Node(node_path.toNode(&src_model)->getName()));
                    primary.setIsPrimary(true);
                    primary_node_path = node_path.parent().plus(
                            node_path.parent().toNode(&dst_model)->getChildCount() - 1);
                }
            } // 特別ケース ここまで
            return primary_node_path;
        }

        /// PrimaryIndex相当のノードの子に、AtomicIndex相当のノードを作ります。
        void addAtomicsInMesh(
                NodePath node_path, const Model& src_model, Model& dst_model, int primary_id, CityObjectIndexSet& indices_in_mesh, bool primary_node_created,
                NodePath& primary_node_path) {
            for (const auto& id: indices_in_mesh.get()) {
                if (id.primary_index != primary_id) continue;

                // 制作済みのものはスキップ
                if (id.atomic_index == CityObjectIndex::invalidIndex() && primary_node_created) continue;

                const auto src_node = node_path.toNode(&src_model);
                const auto src_mesh = src_node->getMesh();
                const auto src_city_obj_list = src_mesh->getCityObjectList();

                const static std::string default_gml_id = "gml_id_not_found";
                std::string atomic_gml_id = default_gml_id;
                src_city_obj_list.tryGetAtomicGmlID(id, atomic_gml_id);
                std::string node_name = atomic_gml_id == default_gml_id ? src_node->getName() : atomic_gml_id;

                auto atomic_mesh = FilterByCityObjIndex().filter(*src_mesh, id, 0);
                if (atomic_mesh.hasVertices() || atomic_mesh.getCityObjectList().size() > 0) {
                    // ここでノードを追加します。
                    auto atomic_node_path = primary_node_path.addChildNode(Node(node_name), &dst_model);

                    // 最小地物のGML IDを記録します。
                    atomic_mesh.setCityObjectList({{{{0, 0}, atomic_gml_id}}});
                    // 親となる主要地物のGML IDを記録します。実際は最小地物のみのMeshであっても、対応する主要地物を記憶しておきます。
                    std::string primary_gml_id = default_gml_id;
                    if (src_city_obj_list.tryGetPrimaryGmlID(id.primary_index, primary_gml_id)) {
                        // 最小地物を見て、はじめて親の主要地物のGML IDが判別できるケースがあります。
                        auto primary_node = primary_node_path.toNode(&dst_model);
                        if (primary_node != nullptr) {
                            primary_node->setName(primary_gml_id);
                            auto mesh = primary_node->getMesh();
                            if (mesh == nullptr) {
                                auto mesh_tmp = std::make_unique<Mesh>();
                                primary_node->setMesh(std::move(mesh_tmp));
                                mesh = primary_node->getMesh();
                            }
                            auto& primary_city_obj_list = mesh->getCityObjectList();
                            if (!primary_city_obj_list.containsCityObjectIndex(CityObjectIndex(0, -1))) {
                                primary_city_obj_list.add(CityObjectIndex(0, -1), primary_gml_id);
                            }
                        }
                    }
                    const auto primary_id_of_parent_of_atomic = CityObjectIndex(0,
                                                                                CityObjectIndex::invalidIndex());
                    atomic_mesh.getCityObjectList().add(primary_id_of_parent_of_atomic, primary_gml_id);

                    atomic_node_path.toNode(&dst_model)->setMesh(std::make_unique<Mesh>(atomic_mesh));
                }
            }
        }

        bool createPrimaryNode(
                NodePath& node_path, const Model& src_model, Model& dst_model, int primary_id,
                NodePath& primary_node_path) {
            // 親にPrimary Nodeがなく、メッシュがある場合は、Primary Nodeを作ります。
            bool primary_node_created = false;
            auto src_mesh = node_path.toNode(&src_model)->getMesh();
            auto primary_mesh = FilterByCityObjIndex().filter(*src_mesh, CityObjectIndex(primary_id, -1),
                                                              -1);
            if (primary_mesh.hasVertices()) {
                auto src_city_obj_list = node_path.toNode(&src_model)->getMesh()->getCityObjectList();

                const static std::string default_gml_id = "gml_id_not_found";
                std::string primary_gml_id = default_gml_id;
                src_city_obj_list.tryGetPrimaryGmlID(primary_id, primary_gml_id);
                const std::string primary_node_name = primary_gml_id == default_gml_id ?
                                                      node_path.toNode(&src_model)->getName() : primary_gml_id;
                // ここでノードを追加します。
                //                            auto dst_parent = node_path.parent().toNode(&dst_model);
                primary_node_path = node_path.parent().addChildNode(Node(primary_node_name), &dst_model);
                primary_node_path.toNode(&dst_model)->setIsPrimary(true);
                primary_mesh.setCityObjectList({{{{0, -1}, primary_gml_id}}});
                primary_node_path.toNode(&dst_model)->setMesh(std::make_unique<Mesh>(primary_mesh));
                primary_node_created = true;
            }
            return primary_node_created;
        }

        CityObjectIndexSet listAllUV4InMesh(Mesh& mesh) {
            CityObjectIndexSet indices_in_mesh;
            for (const auto& uv4: mesh.getUV4()) {
                auto id = CityObjectIndex::fromUV(uv4);
                indices_in_mesh.insert(id);
            }
            return indices_in_mesh;
        }
    }

    Model ConvertToAtomic::convert(const Model* src) const {
        Model dst_model = Model();

        // 探索キュー
        auto queue = NodeQueue();
        queue.pushRoot(src);

        // 幅優先探索の順番で変換します。
        while (!queue.empty()) {
            auto node_path = queue.pop();
            queue.pushChildren(node_path, src);

            if(!node_path.toNode(src)->hasVertices()) {
                const auto src_node = node_path.toNode(src);
                auto dst_node = node_path.parent().addChildNode(Node(src_node->getName()), &dst_model).toNode(&dst_model);
                dst_node->setIsPrimary(src_node->isPrimary());
                continue;
            }

            const auto src_node = node_path.toNode(src);

            // どのインデックス(uv4)が含まれるかを列挙します。
            const auto src_mesh = src_node->getMesh();
            CityObjectIndexSet indices_in_mesh = listAllUV4InMesh(*src_mesh);
            CityObjectIndexSet primary_indices_in_mesh;
            for(auto& index_in_mesh : indices_in_mesh.get()) {
                primary_indices_in_mesh.insert(CityObjectIndex(index_in_mesh.primary_index, -1));
            }

            const auto& src_city_obj_list = src_mesh->getCityObjectList();

            // PrimaryIndexごとの処理
            for (auto primary_id: primary_indices_in_mesh.get()) {
                NodePath primary_node_path = calcPrimaryNodePathInParent(node_path, *src, dst_model, indices_in_mesh);

                bool primary_node_created = false;
                if (primary_node_path.empty() || primary_node_path == node_path) {
                    primary_node_created = createPrimaryNode(node_path, *src, dst_model, primary_id.primary_index, primary_node_path);
                }

                // ここで主要地物に以下の最小地物を追加
                addAtomicsInMesh(node_path, *src, dst_model, primary_id.primary_index, indices_in_mesh, primary_node_created,
                                 primary_node_path);
            }
        }

        dst_model.eraseEmptyNodes();
        return dst_model;
    }
}
