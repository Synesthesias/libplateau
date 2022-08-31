#include "libplateau_c.h"
#include "city_model_c.h"
#include <plateau/geometry/mesh_extractor.h>
using namespace libplateau;
using namespace plateau::geometry;
extern "C"{

    /**
     * MeshExtractor::extract して結果を out_model に格納します。
     * out_model の delete はDLL利用者の責任です。
     * ただしModelをdeleteすると配下のNode,Meshにもアクセスできなくなるので注意してください。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_extract(
            const CityModelHandle* const city_model_handle,
            const MeshExtractOptions options,
            Model* const out_model){
        API_TRY{
            MeshExtractor::extract(*out_model, *city_model_handle->getCityModelPtr(), options);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
