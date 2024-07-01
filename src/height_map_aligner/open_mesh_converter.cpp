#include <plateau/height_map_alighner/open_mesh_converter.h>
#include <plateau/height_map_alighner/longest_edge_divider_plateau.h>

namespace plateau::heightMapAligner {

    using namespace OpenMesh::Subdivider::Uniform;

    MeshType OpenMeshConverter::toOpenMesh(const plateau::polygonMesh::Mesh* mesh) {

        auto& vertices = mesh->getVertices();
        auto src_vert_count = vertices.size();

        // OpenMeshの頂点を追加します
        MeshType om_mesh;
        std::vector<MeshType::VertexHandle> v_handles(src_vert_count);
        for(int i=0; i<src_vert_count; i++) {
            v_handles.at(i) = om_mesh.add_vertex(MeshType::Point(vertices[i].x, vertices[i].y, vertices[i].z));
        }

        // OpenMeshの面を追加します。
        auto face_count = mesh->getIndices().size() / 3;
        for(int fi = 0; fi < face_count; fi++) {
            std::vector<MeshType::VertexHandle> face_vhandles(3);
            for(int vi = 0; vi < 3; vi++) {
                face_vhandles.at(vi) = v_handles.at(mesh->getIndices().at(fi * 3 + vi));
            }
            om_mesh.add_face(face_vhandles);
        }

        // GameMaterialIDをOpenMeshのプロパティ格納します。
        game_material_id_prop = GameMaterialIDPropT();
        om_mesh.add_property(game_material_id_prop);
        int face_id = 0;
        int sub_mesh_id = 0;
        auto& sub_meshes = mesh->getSubMeshes();
        for(MeshType::FaceIter f_itr = om_mesh.faces_begin(); f_itr != om_mesh.faces_end(); ++f_itr) {
            int game_mat_id = -1;
            if(sub_mesh_id < sub_meshes.size()) {
                auto& sub_mesh = sub_meshes.at(sub_mesh_id);
                game_mat_id = sub_mesh.getGameMaterialID();
                if(face_id * 3 + 2 >= sub_mesh.getEndIndex()) {
                    ++sub_mesh_id; // 次回のループで使うサブメッシュを切り替えます
                }
            }
            om_mesh.property(game_material_id_prop, *f_itr) = game_mat_id;
            ++face_id;
        }

        // UV1とUV4をOpenMeshのプロパティに格納します。
        uv1_prop = UVPropT();
        uv4_prop = UVPropT();
        om_mesh.add_property(uv1_prop);
        om_mesh.add_property(uv4_prop);
        const auto& src_uv1 = mesh->getUV1();
        const auto& src_uv4 = mesh->getUV4();
        int v_id = 0;
        for(MeshType::VertexIter v_itr = om_mesh.vertices_begin(); v_itr != om_mesh.vertices_end(); ++v_itr) {
            om_mesh.property(uv1_prop, *v_itr) = src_uv1.at(v_id);
            om_mesh.property(uv4_prop, *v_itr) = src_uv4.at(v_id);
            ++v_id;
        }

        return om_mesh;
    }

    void OpenMeshConverter::subdivide(plateau::heightMapAligner::MeshType& mesh) {

        auto divider = LongestEdgeDividerPlateau<MeshType>(game_material_id_prop, uv1_prop, uv4_prop);
        divider.attach(mesh);
        divider.set_max_edge_length(max_edge_length); // ここでSubdivision後の最大エッジ長を指定します。
        divider(1); // 1回Subdivisionを実行。PLATEAU向けにカスタマイズした処理を実行します。
        divider.detach();
    }

    void OpenMeshConverter::toPlateauMesh(plateau::polygonMesh::Mesh* p_mesh, const MeshType& om_mesh) {
        // 頂点
        auto& p_vertices = p_mesh->getVertices();
        auto next_vert_count = om_mesh.n_vertices();
        p_vertices.clear();
        p_vertices.reserve(next_vert_count);
        for(MeshType::VertexIter v_itr = om_mesh.vertices_begin(); v_itr != om_mesh.vertices_end(); ++v_itr) {
            MeshType::Point point = om_mesh.point(*v_itr);
            p_vertices.push_back(TVec3d(point[0], point[1], point[2]));
        }

        // indices
        auto next_face_count = om_mesh.n_faces();
        auto& indices = p_mesh->getIndices();
        indices.clear();
        indices.reserve(next_face_count * 3);
        for(MeshType::FaceIter f_itr = om_mesh.faces_begin(); f_itr != om_mesh.faces_end(); ++f_itr) {
            MeshType::ConstFaceVertexIter fv_end = om_mesh.cfv_end(*f_itr);
            for(MeshType::ConstFaceVertexIter fv_itr = om_mesh.cfv_iter(*f_itr); fv_itr != fv_end; ++fv_itr) {
                int idx = fv_itr->idx();
                indices.push_back(idx);
            }
        }

        // GameMaterialIDの設定
        auto game_mat_id = om_mesh.property(game_material_id_prop, *om_mesh.faces_begin());
        auto& submeshes = p_mesh->getSubMeshes();
        submeshes.clear();
        int face_id = 0;
        int submesh_start = 0;
        for(MeshType::FaceIter f_itr = om_mesh.faces_begin(); f_itr != om_mesh.faces_end(); ++f_itr) {
            auto prop_game_mat = om_mesh.property(game_material_id_prop, *f_itr);
            // ゲームマテリアルIDが変わったとき、または最後のときにSubMeshを追加します。
            if((/*prop_game_mat.has_value() && */game_mat_id != prop_game_mat) || (f_itr+1) == om_mesh.faces_end()) {
                submeshes.emplace_back(submesh_start, face_id * 3 - 1, "", nullptr, game_mat_id.has_value() ? game_mat_id.value() : -1);
                game_mat_id = prop_game_mat;
                submesh_start = face_id * 3;
            }
            ++face_id;
        }

        // UV4の設定
        auto& p_uv4 = p_mesh->getUV4();
        p_uv4.clear();
        p_uv4.reserve(next_vert_count);
        for(MeshType::VertexIter v_itr = om_mesh.vertices_begin(); v_itr != om_mesh.vertices_end(); ++v_itr) {
            auto& uv4_opt = om_mesh.property(uv4_prop, *v_itr);
            auto uv4 = uv4_opt.value_or(TVec2f(-999, -999));
            p_uv4.push_back(uv4);
        }
    }
}
