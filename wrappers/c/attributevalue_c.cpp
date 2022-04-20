#include <citygml/attributesmap.h>
#include <string.h>
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

/// attributeValue の文字列としてのバイト数を取得します。
/// 結果は out_str_length に入ります。
/// 用途は値を文字列として取得する前準備です。
LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attribute_value_get_str_length(
        const AttributeValue *const attributeValue,
        int *out_str_length
) {
    API_TRY {
        const std::string value = attributeValue->asString();
        *out_str_length = value.size();
        return APIResult::Success;
    }
    API_CATCH;
    return APIResult::ErrorUnknown;
}

/// attributeValue の Type を enum形式で取得します。
/// 結果は out_attribute_type に入ります。
LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attribute_value_get_type(
        const AttributeValue *const attributeValue,
        AttributeType *out_attribute_type
) {
    API_TRY {
        *out_attribute_type = attributeValue->getType();
        return APIResult::Success;
    }
    API_CATCH;
    return APIResult::ErrorUnknown;
}

/// この属性のタイプが AttributeSet であることを前提とし、
/// 子の　AttributeSet　を取得します。
/// 結果は out_attribute_set_ptr に入ります。
LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attribute_as_attribute_set(
        const AttributeValue *const attributeValue,
        AttributesMap **out_attribute_set_ptr
) {
    API_TRY {
        auto *attr_set = &attributeValue->asAttributeSet();
        *out_attribute_set_ptr = const_cast<AttributesMap *>(attr_set);
        return APIResult::Success;
    }
    API_CATCH;
    return APIResult::ErrorUnknown;
}
}