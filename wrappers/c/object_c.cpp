#include <iostream>
#include <string.h>
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


    // 正常終了の場合は0, 異常終了の場合はそれ以外を返します。
    LIBPLATEAU_C_EXPORT int LIBPLATEAU_C_API plateau_object_get_attribute(const Object* object, const char* name, char* outStrBuffer, int outStrBufferSize){
        API_TRY{
            auto attrValueStr = object->getAttribute(std::string(name));
            auto attrValueChars = attrValueStr.c_str();
            if(outStrBufferSize < std::strlen(attrValueChars)){
                // バッファーサイズが足りない場合
                return -1;
            }else{
                // バッファーサイズが足りる場合
                int copyResult = strcpy_s(outStrBuffer, outStrBufferSize, attrValueChars);
                return copyResult;
            }
        }
        API_CATCH;
        return -1;
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