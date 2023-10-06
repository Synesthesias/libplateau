#include <plateau/granularity_convert/merge_primary_node_and_children.h>
#include <queue>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    void MergePrimaryNodeAndChildren::merge(const Node& src_node_arg, Mesh& dst_mesh, int primary_id) {
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
}
