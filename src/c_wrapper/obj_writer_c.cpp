#include "libplateau_c.h"
#include <plateau/mesh_writer/obj_writer.h>

using namespace libplateau;
using namespace plateau::meshWriter;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_obj_writer_write(ObjWriter* handle, bool* out,
            const char* obj_file_path, const plateau::polygonMesh::Model* model) {
        API_TRY{
            *out = handle->write(obj_file_path, *model);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_obj_writer(ObjWriter** obj_writer) {
        API_TRY{
            *obj_writer = new ObjWriter;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_obj_writer(ObjWriter* obj_writer) {
        API_TRY{
            delete obj_writer;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}