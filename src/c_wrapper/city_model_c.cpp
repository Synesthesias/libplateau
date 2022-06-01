#include <iostream>

#include "libplateau_c.h"
#include "city_model_c.h"
using namespace libplateau;
using namespace citygml;

extern "C" {
    // CityModelHandleの生成はplateau_load_citygmlから行われます。

    // TODO CityModel::getCityObjectsOfType のラッパーも作っておくと便利かもしれません


    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_delete_city_model(
        const CityModelHandle* city_model_handle
    ) {
        API_TRY{
            delete city_model_handle;
        }
        API_CATCH;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_city_model_get_root_city_objects(
        const CityModelHandle* city_model_handle, const citygml::CityObject** city_objects, int count
    ) {
        API_TRY{
            const auto & city_model = city_model_handle->getCityModel();
            for (int i = 0; i < count; ++i) {
                city_objects[i] = &city_model.getRootCityObject(i);
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }


    DLL_VALUE_FUNC(plateau_city_model_get_root_city_object_count,
                   CityModelHandle,
                   int,
                   handle->getCityModel().getNumRootCityObjects())

    DLL_PTR_FUNC(plateau_city_model_get_city_object_by_id,
                 CityModelHandle,
                 CityObject,
                 handle->getCityModel().getCityObjectById(std::string(id_chars));
                 if(*out == nullptr) return APIResult::ErrorValueNotFound;,
                 ,const char* const id_chars)

}
