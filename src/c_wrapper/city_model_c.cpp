#include <iostream>

#include <citygml/cityobject.h>

#include "libplateau_c.h"
#include "city_model_c.h"
using namespace libplateau;
using namespace citygml;

extern "C" {
    // CityModelHandleの生成はplateau_load_citygmlから行われます。

    // 注意 : P/Invoke を使って DLLの利用者から CityModel のアドレスを受け取るとき、
    // Native側で cityModelHandle->getCityModelPtr() を介さないと
    // 正しいアドレスになりません。

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


    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_city_model_get_all_city_objects_of_type(
            const CityModelHandle* city_model_handle, const citygml::CityObject** city_objects, CityObject::CityObjectsType type, int count
        ) {
        API_TRY{
            const auto& city_model = city_model_handle->getCityModel();
            const auto city_object_vector = city_model.getAllCityObjectsOfType(type);
            for (int i = 0; i < count; ++i) {
                city_objects[i] = city_object_vector[i];
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_VALUE_FUNC(plateau_city_model_get_all_city_object_count_of_type,
                   CityModelHandle,
                   int,
                   static_cast<int>(handle->getCityModel().getAllCityObjectsOfType(type).size()),
                   ,citygml::CityObject::CityObjectsType type)

    DLL_PTR_FUNC(plateau_city_model_get_city_object_by_id,
                 CityModelHandle,
                 CityObject,
                 handle->getCityModel().getCityObjectById(std::string(id_chars));
                 if(*out == nullptr) return APIResult::ErrorValueNotFound;,
                 ,const char* const id_chars)

}
