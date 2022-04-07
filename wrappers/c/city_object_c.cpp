#include <iostream>
#include <obj_writer.h>

#include "libplateau_c.h"

using namespace citygml;

extern "C" {
    LIBPLATEAU_C_EXPORT CityObject::CityObjectsType LIBPLATEAU_C_API plateau_city_object_get_type(
        const CityObject* city_object
    ) {
        API_TRY{
            return city_object->getType();
        }
        API_CATCH;
        return static_cast<CityObject::CityObjectsType>(0);
    }
}