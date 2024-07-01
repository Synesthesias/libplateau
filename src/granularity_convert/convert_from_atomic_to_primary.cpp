#include <plateau/granularity_convert/convert_from_atomic_to_primary.h>
#include <plateau/granularity_convert/node_queue.h>
#include "plateau/granularity_convert/merge_primary_node_and_children.h"

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    Model ConvertFromAtomicToPrimary::convert(const Model* src) const {
        auto dst_model = Model();
        auto queue = NodeQueue();
        dst_model.reserveRootNodes(src->getRootNodeCount());
        // ルートノードを探索キューに加えると同時に、dst_modelに同じノードを準備します。
        queue.pushRoot(src);
        for (int i = 0; i < src->getRootNodeCount(); i++) {
            NodePath({i}).addNodeFromSrc(*src, dst_model);
        }

        int gml_id_not_found_count = 0;
        std::string last_primary_gml_id = "dummy__";
        std::vector<NodePath> unmerged_atomics = {};
        // 幅優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードの子を含めて結合します。そのprimary_idは0とします。
        while (!queue.empty()) {
            auto node_path = queue.pop();
            auto src_node = node_path.toNode(src);
            auto dst_node = node_path.toNode(&dst_model);
            dst_node->setIsActive(src_node->isActive());

            // TODO このへんの処理は ConvertFromAtomicToAreaと似ているのでまとめられるかも
            bool should_merge_to_last_node = false;
            bool is_primary_changed = src_node->isPrimary();
            if (src_node->getMesh() != nullptr) {
                auto mesh = src_node->getMesh();
                auto& city_obj_list = mesh->getCityObjectList();
                auto primaries = city_obj_list.getAllPrimaryIndices();
                if (!primaries.empty()) {
                    auto primary_gml_id = city_obj_list.getPrimaryGmlID(primaries.at(0).primary_index);
                    if (primary_gml_id == "gml_id_not_found") {// TODO マジックナンバー
                        primary_gml_id += "_" + std::to_string(gml_id_not_found_count);
                        gml_id_not_found_count++;
                    }
                    if (primary_gml_id != last_primary_gml_id) {
                        is_primary_changed = true;
                        last_primary_gml_id = primary_gml_id;
                        dst_node->setName(primary_gml_id);
                    } else {
                        should_merge_to_last_node = true;
                    }
                }
            }

            if (should_merge_to_last_node) {
                auto prev_dst_node = node_path.decrement().toNode(&dst_model);
                auto dst_mesh = prev_dst_node->getMesh();
                if (dst_mesh != nullptr) {
                    auto merged_mesh = std::make_unique<Mesh>();
                    MergePrimaryNodeAndChildren().mergeWithChildren(*src_node, *merged_mesh, 0, 0);
                    auto& dst_city_obj_list = dst_mesh->getCityObjectList();
                    int max_atomic_id = 0;
                    for (auto& [id, _]: dst_city_obj_list.getIdMap()) {
                        max_atomic_id = std::max(id.atomic_index, max_atomic_id);
                    }
                    MergePrimaryNodeAndChildren().merge(*merged_mesh, *dst_mesh, CityObjectIndex(0, max_atomic_id + 1));
                    auto& src_city_obj_list = merged_mesh->getCityObjectList();

                }
            } else if (is_primary_changed) {
                // Primaryなら、src_nodeとその子を結合したメッシュをdst_nodeに持たせます。
                auto merged_mesh = std::make_unique<Mesh>();
                MergePrimaryNodeAndChildren().mergeWithChildren(*src_node, *merged_mesh, 0, 0);
                dst_node->setMesh(std::move(merged_mesh));

            } else {
                dst_node->reserveChild(dst_node->getChildCount() + src_node->getChildCount());
                // Primaryに行き着く前にメッシュが見つかった場合、未マージとして保持しておきます。
                // 入力のメッシュがすべて最小地物である場合にありえます。
                if (src_node->getMesh() != nullptr) {
                    unmerged_atomics.push_back(node_path);
                } else {
                    for (int i = 0; i < src_node->getChildCount(); i++) {

                        // 子をキューに加えて探索を続けます。同じ子をdst_nodeに加えます。
                        const auto& src_child = src_node->getChildAt(i);
                        auto& dst_child_node = dst_node->addChildNode(Node(src_child.getName()));
                        dst_child_node.setIsActive(src_node->isActive());
                        queue.push(node_path.plus(i));
                    }
                }
            }
        }

        // FIXME 未マージのものは、とりあえずまとめてルートノード1つにしておきますがもっと適切な方法があるかも……
        auto combined_mesh = std::make_unique<Mesh>();
        int atomic_id = 0;
        for (const auto& unmerged_path: unmerged_atomics) {
            auto src_mesh = unmerged_path.toNode(src)->getMesh();
            MergePrimaryNodeAndChildren().merge(*src_mesh, *combined_mesh, CityObjectIndex(0, atomic_id));
            atomic_id++;
        }
        if (combined_mesh->hasVertices()) {
            auto combined_node = Node("combined");
            combined_node.setMesh(std::move(combined_mesh));
            dst_model.addNode(std::move(combined_node));
        }

        dst_model.eraseEmptyNodes();
        dst_model.optimizeMeshes();
        return dst_model;
    }
}
