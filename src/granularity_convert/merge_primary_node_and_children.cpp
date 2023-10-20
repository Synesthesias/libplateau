#include <plateau/granularity_convert/merge_primary_node_and_children.h>
#include <queue>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    void MergePrimaryNodeAndChildren::mergeWithChildren(const plateau::polygonMesh::Node& src_node_arg, plateau::polygonMesh::Mesh& dst_mesh, int primary_id, int atomic_id_offset) const{
        std::queue<const Node*> queue;
        queue.push(&src_node_arg);
        long next_atomic_id = atomic_id_offset;

        while (!queue.empty()) {
            const auto& src_node = *queue.front();
            queue.pop();

            // メッシュをマージします。
            if (src_node.getMesh() != nullptr && src_node.getMesh()->hasVertices()) {
                // UV4
                int atomic_id;
                if (src_node.isPrimary()) {
                    atomic_id = -1;
                    next_atomic_id = 0;
                } else {
                    atomic_id = next_atomic_id;
                    next_atomic_id++;
                }
                merge(*src_node.getMesh(), dst_mesh, CityObjectIndex(primary_id, atomic_id));
            }

            // 子ノードをキューに入れます。
            for (int i = 0; i < src_node.getChildCount(); i++) {
                queue.push(&src_node.getChildAt(i));
            }
        }
    }

    void MergePrimaryNodeAndChildren::merge(const plateau::polygonMesh::Mesh& src_mesh, plateau::polygonMesh::Mesh& dst_mesh, const plateau::polygonMesh::CityObjectIndex& id) const {
        // 元メッシュをコピーします。重いので注意してください。
        auto src_mesh_copy = Mesh(src_mesh);
        const auto uv4 = id.toUV();
        auto uv4s = std::vector<TVec2f>(src_mesh_copy.getUV4().size(), uv4);
        src_mesh_copy.setUV4(std::move(uv4s));
        // マージします。
        dst_mesh.merge(src_mesh_copy, false, true);

        // CityObjectListを更新します。
        const auto& src_city_obj_list = src_mesh.getCityObjectList();

        // 入力メッシュのgml_idを取得し、CityObjectListに追加します。
        // 入力は最小地物単位であるという前提なので、srcのGML IDを取得できるCityObjectIndexは(0,0)です。
        auto& dst_city_obj_list = dst_mesh.getCityObjectList();
        const static std::string default_gml_id = "gml_id_not_found";
        std::string atomic_gml_id = default_gml_id;
        if(src_city_obj_list.tryGetAtomicGmlID(CityObjectIndex(0, 0), atomic_gml_id)){
            dst_city_obj_list.add(id, atomic_gml_id);
        }


        // srcのCityObjectListのうち、主要地物のものを加えます。
        auto primary_gml_id = default_gml_id;
        if(src_city_obj_list.tryGetPrimaryGmlID(0, primary_gml_id)) {
            // 重複チェックしてから主要地物をCityObjectListに加えます。
            const auto dst_primary_index = CityObjectIndex(id.primary_index, CityObjectIndex::invalidIndex());
            bool contains_primary = dst_city_obj_list.containsPrimaryGmlId(primary_gml_id);

            if(!contains_primary) {
                dst_city_obj_list.add(dst_primary_index, primary_gml_id);
            }
        }
    }
}
