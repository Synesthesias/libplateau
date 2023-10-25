#include <plateau/granularity_convert/convert_from_atomic_to_area.h>
#include <plateau/granularity_convert/node_stack.h>
#include <plateau/granularity_convert/merge_primary_node_and_children.h>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    Model ConvertFromAtomicToArea::convert(const Model* src) const {
        auto dst_model = Model();

        if (src->getRootNodeCount() == 0)
            return dst_model;

        // 深さ優先探索でPrimaryなNodeを探し、Primaryが見つかるたびにそのノードを子を含めて結合し、primary_idをインクリメントします。
        long primary_id = -1;
        std::string last_primary_gml_id = "dummy__";
        int atomic_id_offset = 0;

        Mesh* dst_mesh = nullptr;
        Node* dst_node = nullptr;
        // 探索用スタック
        auto stack = NodeStack();
        stack.pushRoot(src);
        int primary_loop_count = -1;

        // 深さ優先探索でsrcを走査
        while (!stack.empty()) {
            const auto node_path = stack.pop();
            const auto& src_node = node_path.toNode(src);

            bool src_contains_primary = src_node->isPrimary();
            const auto src_mesh = src_node->getMesh();
            if (src_mesh != nullptr) {
                auto& src_city_obj_list = src_mesh->getCityObjectList();
                src_contains_primary |= !src_city_obj_list.getAllPrimaryIndices().empty();
            }

            if (src_contains_primary) {

                primary_loop_count++;
                bool should_increment_primary_id = false;
                if (src_node->getMesh() != nullptr) {
                    auto& src_city_obj_list = src_node->getMesh()->getCityObjectList();
                    auto primaries = src_city_obj_list.getAllPrimaryIndices();
                    if (!primaries.empty()) {
                        auto& primary_gml_id = src_city_obj_list.getPrimaryGmlID(primaries.at(0).primary_index);
                        if (primary_gml_id != last_primary_gml_id) {
                            should_increment_primary_id = true;
                            atomic_id_offset = 0;
                            last_primary_gml_id = primary_gml_id;
                        } else {
                            atomic_id_offset++;
                        }
                    }
                }
                if (src_node->isPrimary()) {
                    should_increment_primary_id = true;
                }
                if (should_increment_primary_id) primary_id++;

                // PrimaryなNodeが見つかったら、そのノードと子をマージします。

                if (dst_mesh == nullptr) { // ループ中、最初にPrimaryであるとき
                    // 最初にdst_meshとdst_nodeを作ります。

                    // 親までのtree構造はsrcとdstで同じはずなのでsrcのpathを利用して親を取得
                    const auto parent = node_path.parent().toNode(&dst_model);

                    if (parent == nullptr) {
                        dst_node = &dst_model.addEmptyNode(src_node->getName());
                    } else {
                        dst_node = &parent->addEmptyChildNode(src_node->getName());
                    }
                    dst_node->setMesh(std::make_unique<Mesh>());
                    dst_mesh = dst_node->getMesh();
                }
                MergePrimaryNodeAndChildren().mergeWithChildren(*src_node, *dst_mesh, primary_id, atomic_id_offset);

                // dst_nodeの名前は、マージ対象のPrimaryが1つの場合はsrc_nodeと同じ名前にすれば良いですが、
                // 2つ以上の場合はそれではそぐわないのでcombinedという名前にします。
                if(primary_loop_count == 1) { // ループ中のPrimaryが2回目のとき
                    dst_node->setName("combined");
                }

            } else {
                // PrimaryなNodeでなければ空のNodeを作成

                // 親までのtree構造はsrcとdstで同じはずなのでsrcのpathを利用して親を取得
                const auto parent = node_path.parent().toNode(&dst_model);

                Node* dst_node;
                if (parent == nullptr) {
                    dst_node = &dst_model.addEmptyNode(src_node->getName());
                } else {
                    dst_node = &parent->addEmptyChildNode(src_node->getName());
                }

                // 格納先が変わるのでPrimary ID, dst_meshはリセット
                primary_id = -1;
                dst_mesh = nullptr;

                // Primaryが不明なAtomic
                if (src_node->getMesh() != nullptr) {
                    // とりあえず詰め込む
                    // TODO: 分離して保持？
                    dst_node->setMesh(std::make_unique<Mesh>());
                    dst_node->getMesh()->merge(*src_node->getMesh(), false, true);
                }

                // 子がなければ終了(本来ならあるはず)
                if (src_node->getChildCount() == 0) {
                    continue;
                }

                // Primaryに行き着くまで探索を続けます。
                // index 0から処理したいため逆順にpush
                for (int i = src_node->getChildCount() - 1; i >= 0; i--) {
                    stack.push(node_path.plus(i));
                }
            }
        }

        // Primary Nodeが不明なAtomicをマージします。
//        int atomic_id = 0;
//        for(const auto& unmerged_node : unmerged_nodes) {
//            const auto src_mesh = unmerged_node.toNode(src)->getMesh();
//            MergePrimaryNodeAndChildren().merge(*src_mesh, dst_mesh, CityObjectIndex(primary_id, atomic_id));
//            auto& dst_city_obj_list = dst_mesh.getCityObjectList();
//            // 主要地物のGML IDを追加
//            if(!dst_city_obj_list.containsCityObjectIndex(CityObjectIndex(primary_id, -1))){
//                auto& src_city_obj_list = src_mesh->getCityObjectList();
//                auto primaries = src_city_obj_list.getAllPrimaryIndices();
//                for(auto& primary : primaries) {
//                    auto& primary_gml_id = src_city_obj_list.getPrimaryGmlID(primary.primary_index);
//                    dst_city_obj_list.add(primary, primary_gml_id);
//                }
//            }
//            atomic_id++;
//        }
        return dst_model;
    }
}
