#include "libplateau_c.h"

#include <plateau/dataset/i_dataset_accessor.h>

using namespace libplateau;
using namespace plateau::dataset;
using namespace plateau::geometry;

extern "C" {

    // MeshCode は構造体としてポインタではない実体をやりとり可能です。

    LIBPLATEAU_C_EXPORT MeshCode LIBPLATEAU_C_API plateau_mesh_code_parse(
        const char* code
    ) {
        return MeshCode(code);
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_code_get_extent(
        const MeshCode mesh_code, Extent* extent
    ) {
        API_TRY{
            *extent = mesh_code.getExtent();
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_2_ARG_FUNC(plateau_mesh_code_is_valid,
                   MeshCode mesh_code,
                   bool* out_is_valid,
                   *out_is_valid  = mesh_code.isValid())
}
