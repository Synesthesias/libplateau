#include "libplateau_c.h"
#include <plateau/polygon_mesh/sub_mesh.h>
#include <citygml/material.h>
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

    DLL_PTR_FUNC(plateau_sub_mesh_get_material,
                SubMesh,
                citygml::Material,
                handle->getMaterial().get())

    DLL_2_ARG_FUNC(plateau_sub_mesh_set_game_material_id,
                   SubMesh* sub_mesh,
                   int game_material_id,
                   sub_mesh->setGameMaterialID(game_material_id))

    DLL_VALUE_FUNC(plateau_sub_mesh_get_game_material_id,
                   SubMesh,
                   int,
                   handle->getGameMaterialID())

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_sub_mesh(
            SubMesh** out_sub_mesh_ptr,
            const int start_index,
            const int end_index,
            const char* const texture_path
    ) {
        *out_sub_mesh_ptr = new SubMesh(start_index, end_index, texture_path, nullptr);
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_sub_mesh(const SubMesh* sub_mesh) {
        delete sub_mesh;
        return APIResult::Success;
    }
}
