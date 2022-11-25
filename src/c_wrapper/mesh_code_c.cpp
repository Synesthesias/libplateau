#include "libplateau_c.h"

#include <plateau/dataset/local_dataset_accessor.h>

using namespace libplateau;
using namespace plateau::dataset;
using namespace plateau::geometry;

extern "C" {
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
}
