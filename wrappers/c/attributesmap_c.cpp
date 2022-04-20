#include <citygml/attributesmap.h>
#include <string.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

    /// AttributesMapの要素数を返します。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_get_key_count(
            const AttributesMap *const attributesMap,
            int* out_count) {
        API_TRY {
            *out_count = attributesMap->size();
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    /// AttributesMapの各キーの文字列のバイト数をint配列 out_sizes に格納します。
    /// out_sizes は AttributesMap の要素数以上のメモリが確保されていることが前提であり、そうでないとアクセス違反です。
    /// DLLで文字列をやりとりするときに長さ情報が欲しいための関数です。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_get_key_sizes(
            const AttributesMap *const attributesMap,
            int *const out_sizes) {

        API_TRY {
            int i = 0;
            for (const auto &pair: *attributesMap) {
                out_sizes[i] = pair.first.size();
                i++;
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    /// AttributesMapの各キーを文字列の配列 out_keys に格納します。
    /// DLLの利用者が out_keys の各文字列ポインタから何バイト読み出せば良いかを知るには
    /// 上の関数から要素数と各keyのバイト数を取得すれば良いです。
    /// out_keys の各要素が必要なメモリを確保していなければアクセス違反となります。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_get_keys(
            const AttributesMap *const attributesMap,
            char **out_keys
    ) {

        API_TRY {
            int i = 0;
            for (const auto &pair: *attributesMap) {
                char *string_i = out_keys[i];
                strcpy(string_i, pair.first.c_str());
                i++;
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }


    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_get_attribute_value(
            const AttributesMap *const attributesMap,
            const char *const key_char,
            const AttributeValue** out_attribute_value_ptr
    ) {
        API_TRY {
            auto key_str = std::string(key_char);
            if(attributesMap->count(key_str) <= 0){
                return APIResult::ErrorValueNotFound;
            }
            const AttributeValue *value = &(*attributesMap).at(key_str);
            *out_attribute_value_ptr = value;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_do_contains_key(
            const AttributesMap* const attributesMap,
            const char* const key_char,
            bool* out_result
            ){
        API_TRY{
            auto key_str = std::string(key_char);
            bool result = attributesMap->count(key_str) > 0;
            *out_result = result;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }


    /// AttributeValue の属性値（文字列）を out_value に格納します。
    /// out_value には値を格納するのに十分なメモリが確保されていることが前提であり、
    /// そうでなければアクセス違反となります。
    /// 何バイトのメモリが必要であるかをDLLの利用者が知るには、
    /// plateau_attribute_value_get_str_length 関数を利用します。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attribute_value_get_string(
            const AttributeValue *const attributeValue,
            char *out_value
    ) {
        API_TRY {
            const std::string value_str = attributeValue->asString();
            strcpy(out_value, value_str.c_str());
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    /// attributeValue の文字列としてのバイト数を返します。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attribute_value_get_str_length(
            const AttributeValue *const attributeValue,
            int* out_str_length
    ) {
        API_TRY {
            const std::string value = attributeValue->asString();
            *out_str_length = value.size();
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attribute_value_get_type(
            const AttributeValue *const attributeValue,
            AttributeType* out_attribute_type
    ) {
        API_TRY {
            *out_attribute_type = attributeValue->getType();
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attribute_as_attribute_set(
            const AttributeValue *const attributeValue,
            AttributesMap** out_attribute_set_ptr
    ) {
        API_TRY {
            auto* attr_set = &attributeValue->asAttributeSet();
            *out_attribute_set_ptr = const_cast<AttributesMap*>(attr_set);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}