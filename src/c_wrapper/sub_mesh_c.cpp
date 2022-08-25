#include "libplateau_c.h"
#include <plateau/geometry/sub_mesh.h>
using namespace libplateau;
using namespace plateau::geometry;
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
}