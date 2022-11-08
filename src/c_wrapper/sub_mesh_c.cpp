#include "libplateau_c.h"
#include <plateau/polygon_mesh/sub_mesh.h>
using namespace libplateau;
using namespace plateau::polygonMesh;
extern "C"{

    DLL_VALUE_FUNC(plateau_sub_mesh_get_start_index,
                   SubMesh,
                   int,
                   handle->getStartIndex())

    DLL_VALUE_FUNC(plateau_sub_mesh_get_end_index,
                   SubMesh,
                   int,
                   handle->getEndIndex())

    DLL_STRING_PTR_FUNC(plateau_sub_mesh_get_texture_path,
                        SubMesh,
                        handle->getTexturePath())

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_sub_mesh(
            SubMesh** out_sub_mesh_ptr,
            const int start_index,
            const int end_index,
            const char* const texture_path
    ) {
        *out_sub_mesh_ptr = new SubMesh(start_index, end_index, texture_path);
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_sub_mesh(const SubMesh* sub_mesh) {
        delete sub_mesh;
        return APIResult::Success;
    }
}
