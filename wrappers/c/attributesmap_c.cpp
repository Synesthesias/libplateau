#include <citygml/attributesmap.h>
#include <string.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

    /// AttributesMapの要素数を取得します。結果は out_count のメモリ領域に書き込まれます。
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

    /// AttributesMapの各キーの文字列のバイト数(null終端文字を含む)をint配列 out_sizes に格納します。
    /// out_sizes は AttributesMap の要素数以上のメモリが確保されていることが前提であり、そうでないとアクセス違反です。
    /// DLLで文字列をやりとりするときに長さ情報が欲しいための関数です。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_get_key_sizes(
            const AttributesMap *const attributesMap,
            int *const out_sizes) {

        API_TRY {
            int i = 0;
            for (const auto &pair: *attributesMap) {
                auto& key = pair.first;
                out_sizes[i] = key.size() + 1; // +1 は null終端文字の分です。
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
                // キー文字列を out_keys[i] にコピーします。
                char *string_i = out_keys[i];
                auto key_str = pair.first.c_str();
                int len = strlen(key_str);
                strncpy(string_i, key_str, len);
                string_i[len] = '\0'; // 最後はnull終端文字
                i++;
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }


    /// AttributeMap の key に対応する AttributeValue のポインタを取得します。
    /// 結果は out_attribute_value_ptr のメモリ領域に入ります。
    /// key に対応するものがない場合は APIResult::ErrorValueNotFound を返します。
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

    /// attributeMap で key_char に対応するものがあるかをチェックします。
    /// あるなら out_result に true が入り、ないなら false が入ります。
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


}