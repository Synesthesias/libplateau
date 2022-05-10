#include <iostream>

#include "libplateau_c.h"
#include "city_model_c.h"
using namespace libplateau;

extern "C" {
    // CityModelHandleの生成はplateau_load_citygmlから行われます。

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

    LIBPLATEAU_C_EXPORT int LIBPLATEAU_C_API plateau_city_model_get_root_city_object_count(
        const CityModelHandle* city_model_handle
    ) {
        API_TRY{
            const auto & city_model = city_model_handle->getCityModel();
            return city_model.getNumRootCityObjects();
        }
        API_CATCH;
        return 0;
    }
}
