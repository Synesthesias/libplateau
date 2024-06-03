#include <plateau/height_map_alighner/open_mesh_converter.h>

namespace plateau::heightMapAligner {
    MeshType OpenMeshConverter::toOpenMesh(const plateau::polygonMesh::Mesh* mesh) {

        auto& vertices = mesh->getVertices();
        auto src_vert_count = vertices.size();

        // OpenMeshの頂点を追加します
        MeshType om_mesh;
        std::vector<MeshType::VertexHandle> v_handles(src_vert_count);
        for(int i=0; i<src_vert_count; i++) {
            v_handles.at(i) = om_mesh.add_vertex(MeshType::Point(vertices[i].x, vertices[i].y, vertices[i].z));
        }

        // OpenMeshの面を追加します
        auto face_count = mesh->getIndices().size() / 3;
        for(int fi = 0; fi < face_count; fi++) {
            std::vector<MeshType::VertexHandle> face_vhandles(3);
            for(int vi = 0; vi < 3; vi++) {
                face_vhandles.at(vi) = v_handles.at(mesh->getIndices().at(fi * 3 + vi));
            }
            om_mesh.add_face(face_vhandles);
        }
        return om_mesh;
    }

    void OpenMeshConverter::toPlateauMesh(plateau::polygonMesh::Mesh* p_mesh, const MeshType& om_mesh) {
        auto& p_vertices = p_mesh->getVertices();
        auto next_vert_count = om_mesh.n_vertices();
        p_vertices.clear();
        p_vertices.reserve(next_vert_count);
        for(MeshType::VertexIter v_itr = om_mesh.vertices_begin(); v_itr != om_mesh.vertices_end(); ++v_itr) {
            MeshType::Point point = om_mesh.point(*v_itr);
            p_vertices.push_back(TVec3d(point[0], point[1], point[2]));
        }
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
        auto indices_count = indices.size();
        p_mesh->extendLastSubMesh(indices_count-1);
    }
}
