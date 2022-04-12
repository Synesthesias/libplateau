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


    // 属性名から属性値を取得し、 outStrBuffer に格納します。
    // 結果の成否を戻り値にします。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_object_get_attribute(const Object* object, const char* name, char* outStrBuffer, int outStrBufferSize){
        API_TRY{
            auto attrValueStr = object->getAttribute(std::string(name));
            auto attrValueChars = attrValueStr.c_str();
             if(attrValueStr.empty()){
                 // 属性が存在しない場合は空文字列が返るので、空文字列のときに異常終了とします。
                 return APIResult::ErrorValueNotFound;
             }
            if(outStrBufferSize < strlen(attrValueChars)){
                // バッファーサイズが足りない場合
                return APIResult::ErrorLackOfBufferSize;
            }else{
                // バッファーサイズが足りる場合
                
                // strcpy を使うと、MSVC環境では 「strcpy_s を使うべき」と警告がでます。
                // しかし、MSVC環境でなければ strcpy_s は利用できずビルドが通らないためこの警告は無視します。
                strcpy(outStrBuffer, attrValueChars);
                return APIResult::Success;
            }
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    /// 属性と値の一覧を chars* 型で取得します。
    /// key1, value1, key2, value2, ... の順番で改行区切りで keys_values に格納します。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_object_get_keys_values(const Object* object, char* out_keys_values, int outBufferSize){
        API_TRY {
            auto &attrs = object->getAttributes();
            // 属性の一覧を文字列に変換します。
            auto keys_vals_str = std::string("");
            for (auto attr: attrs) {
                const auto &key = attr.first;
                const auto &value = attr.second.asString();
                keys_vals_str += key;
                keys_vals_str += "\n";
                keys_vals_str += value;
                keys_vals_str += "\n";
            }
            // 最後の改行を削除します。
            if (!keys_vals_str.empty()) keys_vals_str.pop_back();

            auto chars = keys_vals_str.c_str();
            if (outBufferSize >= strlen(chars)) {
                // 取得した属性文字列をコピーして終了します。
                strcpy(out_keys_values, chars);
                return APIResult::Success;
            }else{
                // バッファサイズが足りない場合
                return APIResult::ErrorLackOfBufferSize;
            }
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
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