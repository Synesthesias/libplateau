#include <citygml/attributesmap.h>
#include <cstring>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

DLL_STRING_PTR_FUNC(plateau_attribute_value_get_string,
                       AttributeValue,
                       handle->asString())


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