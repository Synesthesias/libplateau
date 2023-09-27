#include "libplateau_c.h"
#include <plateau/granularity_convert/granularity_converter.h>
#include <plateau/polygon_mesh/model.h>
using namespace plateau::granularityConvert;
using namespace plateau::polygonMesh;
using namespace libplateau;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_granularity_converter_convert(
            const Model* const src_model,
            Model** out_model_ptr,
            const GranularityConvertOption option
            ){
        API_TRY {
            auto converter = GranularityConverter();
            auto* converted_model = new Model(converter.convert(*src_model, option));
            *out_model_ptr = converted_model;
            return APIResult::Success;
        }API_CATCH
        return APIResult::ErrorUnknown;
    }
}
