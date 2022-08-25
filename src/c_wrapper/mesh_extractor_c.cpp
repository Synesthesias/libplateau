#include "libplateau_c.h"
#include "city_model_c.h"
#include <plateau/geometry/mesh_extractor.h>
using namespace libplateau;
using namespace plateau::geometry;
extern "C"{

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_new(
            MeshExtractor** const out_mesh_extractor_ptr
            ){
        *out_mesh_extractor_ptr = new MeshExtractor();
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_delete(
            MeshExtractor* mesh_extractor
            ){
        delete mesh_extractor;
        return APIResult::Success;
    }


    /**
     * MeshExtractor::extract して結果を out_model_ptr に格納します。
     * out_model_ptr は new した生ポインタになるので、deleteはDLL利用者の責任です。
     * ただしModelをdeleteすると配下のNode,Meshにもアクセスできなくなるので注意してください。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_extract(
            const MeshExtractor* const mesh_extractor,
            const CityModelHandle* const city_model_handle,
            const MeshExtractOptions options,
            Model** const out_model_ptr){
        API_TRY{
            auto modelPtr =
                    mesh_extractor->extract_to_row_pointer(*city_model_handle->getCityModelPtr(), options);
            *out_model_ptr = modelPtr;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}