#pragma once
#include <string>

#include <citygml/citygml.h>
#include <libplateau_api.h>
#include <plateau/polygon_mesh/mesh_extractor.h>

namespace Microsoft::glTF {
    struct Scene;
    struct Mesh;
    class Document;
    class BufferBuilder;
}

namespace plateau::meshWriter {
    /**
     * @enum GltfFileFormat
     *
     * 出力ファイルフォーマット
     */
    enum class GltfFileFormat {
        GLB,
        GLTF
    };
    /**
     * \brief glTF出力の設定です。
     */
    struct GltfWriteOptions {

        /**
         * \brief 出力ファイルフォーマットを指定します。
         */
        GltfFileFormat mesh_file_format;

        /**
         * \brief テクスチャファイル用のディレクトリをgltfファイルからの相対パスで指定します。
         * NULLの場合はgltfファイルと同じ階層にテクスチャファイルを保存します。
         */
        std::string texture_directory_path;

        GltfWriteOptions() :
            mesh_file_format(GltfFileFormat::GLB), texture_directory_path("") {
        }
    };

    class LIBPLATEAU_EXPORT GltfWriter {
    public:
        GltfWriter();

        bool write(const std::string& destination, const plateau::polygonMesh::Model& model, GltfWriteOptions options);

    private:
        void precessNodeRecursive(const plateau::polygonMesh::Node& node, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder);
        std::string writeMaterialReference(std::string texUrl, Microsoft::glTF::Document& document);
        void writeNode(Microsoft::glTF::Document& document);
        void writeMesh(std::string accessorIdPositions, std::string accessorIdIndices, std::string accessorIdTexCoords, Microsoft::glTF::BufferBuilder& bufferBuilder);

        std::map<std::string, std::string> required_materials_;
        std::unique_ptr<Microsoft::glTF::Scene> scene_;
        std::unique_ptr<Microsoft::glTF::Mesh> mesh_;
        std::string node_name_;
        int image_id_num_, texture_id_num_;
        std::map<std::string, std::string> material_ids_;
        std::string default_material_id_, current_material_id_;
        GltfWriteOptions options_;
    };
}
