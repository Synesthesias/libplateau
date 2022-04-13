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


    /// �W�I���g���̐���Ԃ��܂��B
    /// ��O���������ꍇ�� -1 ��Ԃ��܂��B
    LIBPLATEAU_C_EXPORT int LIBPLATEAU_C_API plateau_city_object_get_geometries_count(
            const CityObject* const city_object
            ){
        API_TRY{
            return city_object->getGeometriesCount();
        }
        API_CATCH;
        return -1;
    }
}