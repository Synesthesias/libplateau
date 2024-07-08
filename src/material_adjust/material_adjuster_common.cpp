#include <plateau/material_adjust/material_adjuster_common.h>
#include <cmath>

namespace plateau::materialAdjust {
    void MaterialAdjusterCommon::exec(plateau::polygonMesh::Model& model) {
        // 方針:
        // 頂点やUVは変えず、SubMeshesのみ作り直すことでマテリアル分けを実現します。
        // indicesを走査しながらSubMeshを作ります。
        // マテリアル変更の基準は MaterialAdjusterBase のサブクラスに委譲します。

        auto nodes = model.bfsNodes();
        // 各ノードについて
        for (auto node: nodes) {
            auto mesh = node->getMesh();
            if(mesh == nullptr) continue;

            // メッシュについて
            auto& src_obj_list = mesh->getCityObjectList();
            if(src_obj_list.size() == 0) continue;

            auto& src_indices = mesh->getIndices();
            auto& src_uv4 = mesh->getUV4();
            auto& src_sub_meshes = mesh->getSubMeshes();
            auto dst_sub_meshes = std::vector<SubMesh>();
            auto indices_count = mesh->getIndices().size();
            auto src_sub_mesh_count = src_sub_meshes.size();
            dst_sub_meshes.reserve(src_sub_meshes.size());

            auto current_obj_id = CityObjectIndex(-99, -99);
            int current_src_sub_mesh_id = -99;
            // 各ポリゴン（三角形）について
            for(int i=0; i<indices_count; i+=3) {

                auto index_change_num = 0;
                auto next_obj_id = current_obj_id;
                // 三角形内の各頂点をCityObjectIDをチェックします。
                // 3つの頂点のうち2つでCityObjectIndexが変わったら、次のCityObjectIndexに移ったと判断します。
                for(int j=0; j<3; j++) {
                    const auto vert_id = src_indices.at(i+j);
                    if(vert_id >= src_uv4.size()) {
                        break;
                    }
                    const auto uv4 = src_uv4.at(vert_id);
                    const auto obj_id = CityObjectIndex::fromUV(uv4);
                    if(obj_id != current_obj_id) {
                        index_change_num++;
                        next_obj_id = obj_id;
                    }
                }
                const bool obj_id_changed = index_change_num >= 2;

                // CityObjectIndexが変わったなら、マテリアル分けの対象となったかチェックします。
                bool should_override_material = false; // マテリアル分けにより新たなマテリアルに切り替えるかどうかです。
                int next_override_game_mat_id; // should_override_material がtrueのとき、どのマテリアルを設定すべきかです。
                if (obj_id_changed) {
                    current_obj_id = next_obj_id;
                    // CityObjectIndexが変わったとき、マテリアル分けの対象となっているかチェックします。
                    std::string gml_id;
                    if(src_obj_list.tryGetAtomicGmlID(current_obj_id, gml_id)) {
                        should_override_material = base->shouldOverrideMaterial(gml_id, next_override_game_mat_id);
                    }
                }

                // srcのサブメッシュが変わったかどうか確認します。
                const bool src_sub_mesh_changed =
                        current_src_sub_mesh_id < (int)src_sub_mesh_count &&
                        (
                                current_src_sub_mesh_id < 0 ||
                                src_sub_meshes.at(current_src_sub_mesh_id).getEndIndex() <= i
                        );
                if(src_sub_mesh_changed) {
                    current_src_sub_mesh_id = std::max(current_src_sub_mesh_id+1, 0);
                }

                if(should_override_material || src_sub_mesh_changed) {
                    // 新しいSubMeshを生成します。
                    auto next_sub_mesh = src_sub_meshes.at(current_src_sub_mesh_id);
                    next_sub_mesh.setStartIndex(i);
                    next_sub_mesh.setEndIndex(i + 2); // EndIndex次のループ以降で変更していきます。
                    if(should_override_material) {
                        next_sub_mesh.setGameMaterialID(next_override_game_mat_id);
                    }
                    dst_sub_meshes.push_back(next_sub_mesh);
                }else{
                    // SubMeshが変わらない場合、前のSubMeshを延長します。
                    dst_sub_meshes.at(dst_sub_meshes.size()-1).setEndIndex(i + 2);
                }

            } // END indicesのループ

            // 変更後のSubMeshの適用
            mesh->setSubMeshes(std::move(dst_sub_meshes));
            mesh->combineSameSubMeshes();
        }
    }
}
