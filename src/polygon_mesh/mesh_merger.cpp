#include <cassert>
#include <plateau/polygon_mesh/mesh_merger.h>


namespace plateau::polygonMesh {
    using namespace geometry;

    namespace {
        bool isValidMesh(const Mesh& mesh) {
            return !(mesh.getVertices().empty() || mesh.getIndices().empty());
        }

        /**
         * @brief SubMesh以外の形状情報をマージします。
         */
        void mergeShape(Mesh& mesh, const Mesh& other_mesh, const bool invert_mesh_front_back) {
            const auto vertex_count = mesh.getVertices().size();
            const auto other_vertex_count = other_mesh.getVertices().size();

            mesh.addVerticesList(other_mesh.getVertices());
            mesh.addIndicesList(other_mesh.getIndices(), static_cast<unsigned>(vertex_count), invert_mesh_front_back);
            mesh.addUV1(other_mesh.getUV1(), static_cast<unsigned>(other_vertex_count));
            mesh.addUV4(other_mesh.getUV4(), static_cast<unsigned>(other_vertex_count));
        }

        /**
         * merge関数 のテクスチャあり版です。
         * テクスチャについては、マージした結果、範囲とテクスチャを対応付ける SubMesh が追加されます。
         */
        void mergeWithTexture(Mesh& mesh, const Mesh& other_mesh, const bool invert_mesh_front_back) {
            if (!isValidMesh(other_mesh)) return;
            auto prev_indices_count = mesh.getIndices().size();

            mergeShape(mesh, other_mesh, invert_mesh_front_back);

            const auto& other_sub_meshes = other_mesh.getSubMeshes();
            size_t offset = prev_indices_count;
            for (const auto& other_sub_mesh : other_sub_meshes) {
                const auto& texture_path = other_sub_mesh.getTexturePath();
                size_t start_index = other_sub_mesh.getStartIndex() + offset;
                size_t end_index = other_sub_mesh.getEndIndex() + offset;
                assert(start_index <= end_index);
                assert(end_index < mesh.getIndices().size());
                assert((end_index - start_index + 1) % 3 == 0);
                mesh.addSubMesh(texture_path, start_index, end_index);
            }
        }

        /**
         * merge関数 のテクスチャ無し版です。
         * 生成される Mesh の SubMesh はただ1つであり、そのテクスチャパスは空文字列となります。
         */
        void mergeWithoutTexture(
            Mesh& mesh, const Mesh& other_mesh, const bool invert_mesh_front_back) {

            if (!isValidMesh(other_mesh))
                return;

            mergeShape(mesh, other_mesh, invert_mesh_front_back);
            mesh.extendLastSubMesh(mesh.getIndices().size() - 1);
        }
    }


    void MeshMerger::mergeMesh(
        Mesh& mesh, const Mesh& other_mesh,
        const bool invert_mesh_front_back, const bool include_textures) {

        if (!isValidMesh(other_mesh))
            return;

        if (include_textures) {
            mergeWithTexture(mesh, other_mesh, invert_mesh_front_back);
        } else {
            mergeWithoutTexture(mesh, other_mesh, invert_mesh_front_back);
        }
    }
}
