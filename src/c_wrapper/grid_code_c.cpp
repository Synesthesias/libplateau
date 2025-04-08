#include "libplateau_c.h"
#include <plateau/dataset/grid_code.h>
#include <plateau/dataset/i_dataset_accessor.h>

using namespace libplateau;
using namespace plateau::dataset;
using namespace plateau::geometry;

extern "C" {


    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_grid_code_parse(
        const char* code,
        GridCode** out_grid_code
    ) {
        *out_grid_code = GridCode::createRaw(code);
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_grid_code_get_extent(
        const GridCode* grid_code, Extent* extent
    ) {
        API_TRY{
            if (grid_code == nullptr) return APIResult::ErrorInvalidArgument;
            *extent = grid_code->getExtent();
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_2_ARG_FUNC(plateau_grid_code_is_valid,
                   const GridCode* grid_code,
                   bool* out_is_valid,
                   if (grid_code == nullptr) { *out_is_valid = false; return APIResult::ErrorInvalidArgument; }
                   *out_is_valid = grid_code->isValid())

    DLL_STRING_VALUE_FUNC(plateau_grid_code_get_string_code,
                        GridCode,
                        handle->get())

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_grid_code_delete(
        GridCode* grid_code
    ) {
        delete grid_code;
        return APIResult::Success;
    }
}
