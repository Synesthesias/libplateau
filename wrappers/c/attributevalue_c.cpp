#include <citygml/attributesmap.h>
#include <cstring>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {
/// AttributeValue の属性値（文字列）を out_value に格納します。
/// out_value には値を格納するのに十分なメモリが確保されていることが前提であり、
/// そうでなければアクセス違反となります。
/// 何バイトのメモリが必要であるかをDLLの利用者が知るには、
/// plateau_attribute_value_get_str_length 関数を利用します。
LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attribute_value_get_string(
        const AttributeValue *const attribute_value,
        char *out_value
) {
    API_TRY {
        const std::string value_str = attribute_value->asString();
        // 文字列を out_value にコピーします。
        auto value_chars = value_str.c_str();
        int len = strlen(value_chars);
        strncpy(out_value, value_chars, len);
        out_value[len] = '\0'; // 最後はnull終端文字
        return APIResult::Success;
    }
    API_CATCH;
    return APIResult::ErrorUnknown;
}

/// attribute_value の文字列としてのバイト数(null終端文字を含む)を取得します。
/// 結果は out_str_length に入ります。
/// 用途は値を文字列として取得する前準備です。
LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attribute_value_get_str_length(
        const AttributeValue *const attribute_value,
        int * const out_str_length
) {
    API_TRY {
        const std::string value = attribute_value->asString();
        *out_str_length = value.size() + 1; // +1 は null終端文字の分です。
        return APIResult::Success;
    }
    API_CATCH;
    return APIResult::ErrorUnknown;
}


/// AttributeValue の Type を enum形式で取得します。
DLL_VALUE_FUNC(plateau_attribute_value_get_type,
               AttributeValue,
               AttributeType,
               handle->getType())


/// この属性のタイプが AttributeSet であることを前提とし、
/// 子の　AttributeSet　を取得します。
DLL_PTR_FUNC(plateau_attribute_as_attribute_set,
             AttributeValue,
             AttributesMap,
             &handle->asAttributeSet())
}