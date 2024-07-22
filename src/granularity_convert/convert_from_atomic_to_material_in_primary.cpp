#include <plateau/granularity_convert/convert_from_atomic_to_material_in_primary.h>
#include <plateau/granularity_convert/node_stack.h>
#include "plateau/granularity_convert/merge_primary_node_and_children.h"

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    Model ConvertFromAtomicToMaterialInPrimary::convert(const Model* src) const {
        auto dst_model = Model();
        if(src->getRootNodeCount() == 0)
            return dst_model;

        auto stack = NodeStack();
        stack.pushRoot(src);
        // 深さ優先探索でsrcを走査
        while(!stack.empty()) {
            auto node_path = stack.pop();
            const auto src_node = node_path.toNode(src);
            const auto parent = node_path.parent().toNode(&dst_model);

            // ノードをコピー
            auto dst_node = src_node->copyWithoutChildren();
            if(parent==nullptr) {
                dst_model.addNode(std::move(dst_node));
            } else {
                parent->addChildNode(std::move(dst_node));
            }

            if(src_node->isPrimary()) {
                // Primary Nodeが見つかったら、その子を走査する2段目のループでマテリアルごとに結合します。

                // 2段目ループのスタックを用意します。
                auto stack2 = NodeStack();
                for(int i=(int)src_node->getChildCount() - 1; i>=0; i--) {
                    stack2.push(node_path.plus(i));
                }
                std::map<SubMesh, std::vector<NodePath>, SubMeshCompareByAppearance> sub_mesh_map;

                // Primaryの子に関するループ
                while(!stack2.empty()) {
                    const auto atomic_node_path = stack2.pop();
                    const auto atomic_src_node = atomic_node_path.toNode(src);
                    // 2段目のスタックに追加
                    for(int i=(int)atomic_src_node->getChildCount() - 1; i>=0; i--) {
                        stack2.push(atomic_node_path.plus(i));
                    }
                    
                    const auto atomic_src_mesh = atomic_src_node->getMesh();
                    if(!atomic_src_mesh) continue;
                    auto atomic_sub_meshes = atomic_src_mesh->getSubMeshes();
                    if(atomic_sub_meshes.empty()) continue;
                    auto atomic_sub_mesh = atomic_sub_meshes.at(0);
                    // SubMeshを辞書に追加
                    if(sub_mesh_map.find(atomic_sub_mesh) == sub_mesh_map.end()) {
                        sub_mesh_map.emplace(atomic_sub_mesh, std::vector<NodePath>{atomic_node_path});
                    } else {
                        sub_mesh_map.at(atomic_sub_mesh).push_back(atomic_node_path);
                    }
                }
                // 辞書に応じて結合
                int atomic_id = 0;
                for(auto& [sub_mesh, node_paths]: sub_mesh_map) {
                    auto merged_mesh = std::make_unique<Mesh>();
                    for(auto& atomic_node_path: node_paths) {
                        auto atomic_src_node = atomic_node_path.toNode(src);
                        auto atomic_src_mesh = atomic_src_node->getMesh();
                        if(!atomic_src_mesh) continue;
                        MergePrimaryNodeAndChildren().merge(*atomic_src_mesh, *merged_mesh, CityObjectIndex(0, atomic_id));
                        atomic_id++;
                    }
                    auto merged_node = Node("Material-" + std::to_string(sub_mesh.getGameMaterialID()));
                    merged_node.setMesh(std::move(merged_mesh));
                    node_path.toNode(&dst_model)->addChildNode(std::move(merged_node));
                }

            } else {
                // 1段目のループはPrimaryまで
                for(int i=(int)src_node->getChildCount() - 1; i>=0; i--) {
                    stack.push(node_path.plus(i));
                }
            }
        }
        return dst_model;
    }
}