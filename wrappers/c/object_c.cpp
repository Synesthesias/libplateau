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


    // 属性名から属性値を取得し、 outStrBuffer に格納します。
    // 正常終了の場合は0, 異常終了の場合はそれ以外を返します。
    // 具体的には、属性が存在しないまたは値が空文字列である場合は-1を返し、バッファサイズが足りない場合は-2を返します。
    LIBPLATEAU_C_EXPORT int LIBPLATEAU_C_API plateau_object_get_attribute(const Object* object, const char* name, char* outStrBuffer, int outStrBufferSize){
        API_TRY{
            auto attrValueStr = object->getAttribute(std::string(name));
            auto attrValueChars = attrValueStr.c_str();
             if(attrValueStr.empty()){
                 // 属性が存在しない場合は空文字列が返るので、空文字列のときに異常終了とします。
                 return -1;
             }
            if(outStrBufferSize < strlen(attrValueChars)){
                // バッファーサイズが足りない場合
                return -2;
            }else{
                // バッファーサイズが足りる場合
                
                // strcpy を使うと、MSVC環境では 「strcpy_s を使うべき」と警告がでます。
                // しかし、MSVC環境でなければ strcpy_s は利用できずビルドが通らないためこの警告は無視します。
                strcpy(outStrBuffer, attrValueChars);
                return 0;
            }
        }
        API_CATCH;
        return -99;
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