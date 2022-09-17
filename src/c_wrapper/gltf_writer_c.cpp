#include "libplateau_c.h"
#include <plateau/mesh_writer/gltf_writer.h>

using namespace libplateau;
using namespace plateau::meshWriter;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_gltf_writer_write(GltfWriter* handle, bool* out,
            const char* gltf_file_path, const plateau::polygonMesh::Model* model, const char* tex_path, MeshFileFormat format) {
        API_TRY{
            plateau::meshWriter::GltfWriteOptions gltf_options;
            gltf_options.texture_directory_path = tex_path;
            gltf_options.mesh_file_format = format;
            *out = handle->write(gltf_file_path, *model, gltf_options);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_gltf_writer(GltfWriter** gltf_writer) {
        API_TRY{
            *gltf_writer = new GltfWriter;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_gltf_writer(GltfWriter* gltf_writer) {
        API_TRY{
            delete gltf_writer;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
