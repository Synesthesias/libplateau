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

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_object_get_attribute(
        const Object* object,
        const char* name,
        char* outStrBuffer,
        int outStrBufferSize
    ) {
        auto attrValueStr = object->getAttribute(std::string(name));
        auto attrValueChars = attrValueStr.c_str();

        // TODO バッファーサイズが足りない場合のハンドリング
        strcpy_s(outStrBuffer, outStrBufferSize, attrValueChars);
    }


    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_object_set_attribute(
        Object* object,
        const char* name,
        const char* value,
        const AttributeType type,
        bool overwrite
    ) {
        API_TRY{
            object->setAttribute(name, value, type, overwrite);
        }
        API_CATCH;
    }

    
}