#include <plateau/height_map_alighner/height_map_aligner.h>
#define _USE_MATH_DEFINES
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include "OpenMesh/Tools/Subdivider/Uniform/LongestEdgeT.hh"

using namespace plateau::polygonMesh;
using namespace OpenMesh;


namespace plateau::heightMapAligner {
    typedef TriMesh_ArrayKernelT<>  MeshType;

    namespace {

        void alignMesh(Mesh* mesh, const HeightMapFrame& map) {
            auto& vertices = mesh->getVertices();
            auto src_vert_count = vertices.size();

            // OpenMeshのSubdivision機能を使いたいので、OpenMeshのメッシュを作成します。
            // OpenMeshについてはこちらを参照してください: https://www.graphics.rwth-aachen.de/media/openmesh_static/Documentations/OpenMesh-6.1-Documentation/a00046.html

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

            // OpenMeshでSubdivisionします
            auto divider = Subdivider::Uniform::LongestEdgeT<MeshType>();
            divider.attach(om_mesh);
            divider.set_max_edge_length(5);
            divider(1); // 1回Subdivisionを実行
            divider.detach();

            // OpenMeshからMeshに直します
            auto next_vert_count = om_mesh.n_vertices();
            vertices.clear();
            vertices.reserve(next_vert_count);
            for(MeshType::VertexIter v_itr = om_mesh.vertices_begin(); v_itr != om_mesh.vertices_end(); ++v_itr) {
                MeshType::Point point = om_mesh.point(*v_itr);
                vertices.push_back(TVec3d(point[0], point[1], point[2]));
            }
            auto next_face_count = om_mesh.n_faces();
            auto& indices = mesh->getIndices();
            indices.clear();
            indices.reserve(next_face_count * 3);
            for(MeshType::FaceIter f_itr = om_mesh.faces_begin(); f_itr != om_mesh.faces_end(); ++f_itr) {
                MeshType::FaceVertexIter fv_end = om_mesh.fv_end(*f_itr);
                for(MeshType::FaceVertexIter fv_itr = om_mesh.fv_iter(*f_itr); fv_itr != fv_end; ++fv_itr) {
                    int idx = fv_itr->idx();
                    indices.push_back(idx);
                }
            }
            int indices_count = indices.size();
            mesh->extendLastSubMesh(indices_count-1);

            // 高さをハイトマップに合わせます
//            for(auto& vertex : vertices) {
//                vertex.y = map.posToHeight(TVec2d(vertex.x, vertex.z));
//            }
        }

        void alignNode(Node& node, const HeightMapFrame& map) {
            auto mesh = node.getMesh();
            if(mesh == nullptr) return;
            alignMesh(mesh, map);
        }

        void alignRecursive(Node& node, const HeightMapFrame& map) {
            alignNode(node, map);
            for(int i=0; i<node.getChildCount(); i++) {
                auto& child = node.getChildAt(i);
                alignRecursive(child, map);
            }
        }
    } // END 無名namespace

    void HeightMapAligner::align(Model& model, const HeightMapFrame& map) {
        for(int i=0; i<model.getRootNodeCount(); i++){
            auto& node = model.getRootNodeAt(i);
            alignRecursive(node, map);
        }
    }

    float HeightMapFrame::posToHeight(TVec2d pos) const {
        int map_x = (int)((pos.x - min_x) / (max_x - min_x) * map_width);
        int map_y = (int)((pos.y - min_y) / (max_y - min_y) * map_height);
        map_x = std::max(0, map_x);
        map_y = std::max(0, map_y);
        map_x = std::min(map_x, map_width - 1);
        map_y = std::min(map_y, map_height - 1);
        auto map_val = heightmap[map_y * map_width + map_x];
        return min_height + (max_height - min_height) * (float)map_val / std::numeric_limits<decltype(map_val)>::max();
    }
}
