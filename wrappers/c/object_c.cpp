#include <iostream>
#include <obj_writer.h>

#include "libplateau_c.h"

using namespace citygml;

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

    LIBPLATEAU_C_EXPORT const char* LIBPLATEAU_C_API plateau_object_get_attribute(
        const Object* object
        const char* name
    ) {
        return object->getAttribute(std::string(name)).c_str();
    }


    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_object_set_attribute(
        const Object* object,
        const char* name,
        const char* value,
        const AttributeType type,
        bool overwrite
    ) {
        object->getAttribute(
            std::string(name),
            std::string(value),
            type, overrite
        );
    }

    
}