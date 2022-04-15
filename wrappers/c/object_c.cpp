#include <iostream>
#include <obj_writer.h>

#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {
    LIBPLATEAU_C_EXPORT const char* LIBPLATEAU_C_API plateau_object_get_id(
        const Object* object
    ) {
        API_TRY{
            return object->getId().c_str();
        }
        API_CATCH;
        return nullptr;
    }

    LIBPLATEAU_C_EXPORT AttributesMap* LIBPLATEAU_C_API plateau_object_get_attributes_map(
            const Object* object
            ){
        API_TRY{
            return &const_cast<AttributesMap&>(object->getAttributes());
        }
        API_CATCH;
        return nullptr;
    }

    
}