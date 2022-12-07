#include <citygml/attributesmap.h>
#include "libplateau_c.h"
#include <sstream>

using namespace citygml;
using namespace libplateau;

extern "C" {

    DLL_STRING_PTR_ARRAY_FUNC2(plateau_attributes_map_get_keys,
                      AttributesMap,
                      handle->size(),
                      const auto& pair : *handle,
                      pair.first)

    /// AttributeMap の key に対応する AttributeValue のポインタを取得します。
    /// 結果は out_attribute_value_ptr (arg_3) のメモリ領域に入ります。
    /// key に対応するものがない場合は APIResult::ErrorValueNotFound を返します。
    DLL_3_ARG_FUNC(plateau_attributes_map_get_attribute_value,
                       const AttributesMap* const, // attributes_map
                       const char* const, // key_char
                       const AttributeValue** const, // out_attribute_value_ptr

                   const auto key_str = std::string(arg_2);
                   if (arg_1->count(key_str) <= 0) {
                       return APIResult::ErrorValueNotFound;
                   }
                   *arg_3 = &arg_1->at(key_str);
    )

    /// attributeMap で key_char (arg_2) に対応するものがあるかをチェックします。
    /// あるなら out_result (arg_3) に true が入り、ないなら false が入ります。
    DLL_3_ARG_FUNC(plateau_attributes_map_do_contains_key,
                   const AttributesMap* const, // attributes_map
                   const char* const, // key_char
                   bool* const, // out_result
                   *arg_3 = arg_1->count(std::string(arg_2)) > 0;)


    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_to_string_size(
            const AttributesMap* const handle,
            int* const out
            ){
        API_TRY{
            std::stringstream ss;
            ss << *handle;
            *out = (int) ss.str().length() + 1;
            return APIResult::Success;
        }API_CATCH
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_attributes_map_to_string(
            const AttributesMap* const handle,
            char* const out_str_ptr
            ){
        API_TRY{
            std::stringstream ss;
            ss << *handle;
            auto str = ss.str();
            auto chars = str.c_str();
            auto len = (dll_str_size_t) (str.length());
            strncpy(out_str_ptr, chars, len);
            out_str_ptr[len] = '\0';
            return APIResult::Success;
        }API_CATCH
        return APIResult::ErrorUnknown;
    }

}
