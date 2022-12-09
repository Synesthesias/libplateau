#include "libplateau_c.h"
#include <plateau/dataset/city_model_package.h>

using namespace libplateau;
using namespace plateau::dataset;

extern "C" {

    DLL_4_ARG_FUNC(plateau_create_city_model_package_info,
                   CityModelPackageInfo** out_info_ptr,
                   bool has_appearance,
                   int min_lod,
                   int max_lod,
                   *out_info_ptr = new CityModelPackageInfo(has_appearance, min_lod, max_lod); )


    DLL_DELETE_FUNC(plateau_delete_city_model_package_info,
                    CityModelPackageInfo)

    DLL_VALUE_FUNC(plateau_city_model_package_info_get_has_appearance,
                   CityModelPackageInfo,
                   bool,
                   handle->hasAppearance())

    DLL_VALUE_FUNC(plateau_city_model_package_info_get_min_lod,
                  CityModelPackageInfo,
                  int,
                  handle->minLOD())

    DLL_VALUE_FUNC(plateau_city_model_package_info_get_max_lod,
                   CityModelPackageInfo,
                   int,
                   handle->maxLOD())

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_city_model_package_info_get_predefined(
           PredefinedCityModelPackage predefined,
           bool* out_has_appearance,
           int* out_min_lod, int* out_max_lod
           ){
        API_TRY{
            auto info = CityModelPackageInfo::getPredefined(predefined);
            *out_has_appearance = info.hasAppearance();
            *out_min_lod = info.minLOD();
            *out_max_lod = info.maxLOD();
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
