#include <plateau/granularity_convert/filter_by_city_obj_index.h>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;
    Mesh FilterByCityObjIndex::filter(const Mesh& src, CityObjectIndex filter_id, const int uv4_atomic_index) {
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
}
