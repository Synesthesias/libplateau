#include <citygml/attributesmap.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

    DLL_STRING_PTR_ARRAY_FUNC2(plateau_attributes_map_get_keys,
                      AttributesMap,
                      handle->size(),
                      const auto& pair : *handle,
                      pair.first)

    /// AttributeMap の key に対応する AttributeValue のポインタを取得します。
    /// 結果は out_attribute_value_ptr のメモリ領域に入ります。
    /// key に対応するものがない場合は APIResult::ErrorValueNotFound を返します。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_get_attribute_value(
            const AttributesMap *const attributes_map,
            const char *const key_char,
            const AttributeValue** const out_attribute_value_ptr
    ) {
        API_TRY {
            const auto key_str = std::string(key_char);
            if(attributes_map->count(key_str) <= 0){
                return APIResult::ErrorValueNotFound;
            }
            *out_attribute_value_ptr = &attributes_map->at(key_str);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    /// attributeMap で key_char に対応するものがあるかをチェックします。
    /// あるなら out_result に true が入り、ないなら false が入ります。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_do_contains_key(
            const AttributesMap* const attributes_map,
            const char* const key_char,
            bool* const out_result
            ){
        API_TRY{
            const auto key_str = std::string(key_char);
            const bool result = attributes_map->count(key_str) > 0;
            *out_result = result;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

//    DLL_STRING_PTR_FUNC(plateau_attributes_map_to_string,
//                        AttributesMap,
//                        AttributeValue::AttributesMapToString(*handle))

    DLL_STRING_VALUE_FUNC(plateau_attributes_map_to_string,
                          AttributesMap,
                          AttributeValue::attributesMapToString(*handle))




//    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_to_string(
//            const AttributesMap *const handle,
//            const char **const out_chars_ptr,
//            dll_str_size_t *out_str_length){
//        API_TRY{
//            auto str = (AttributeValue::AttributesMapToString(*handle));
//            *out_chars_ptr = str.c_str();
//            *out_str_length = (dll_str_size_t)str.length() + 1;
//            return APIResult::Success;
//        }
//        API_CATCH;
//        return APIResult::ErrorUnknown;
//    }
}