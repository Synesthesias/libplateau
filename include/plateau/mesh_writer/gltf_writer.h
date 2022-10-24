﻿#pragma once
#include <string>

#include <citygml/citygml.h>
#include <libplateau_api.h>
#include <plateau/polygon_mesh/mesh_extractor.h>

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
        ~GltfWriter();

        bool write(const std::string& destination, const plateau::polygonMesh::Model& model, GltfWriteOptions options);

    private:
        class impl;
        std::unique_ptr<impl> pimpl;
    };
}
