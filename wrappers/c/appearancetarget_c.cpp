#include <citygml/appearancetarget_c.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

    DLL_VALUE_FUNC(plateau_appearance_target_get_all_texture_themes_count,
                   AppearanceTarget,
                   int,
                   handle->getAllTextureThemes(front).size(),
                   ,bool front)

DLL_STRINGS_SIZE_ARRAY(plateau_appearance_target_get_all_texture_themes_str_sizes,
                       AppearanceTarget,
                       const auto& str : handle->getAllTextureThemes(front),
                       str,
                       ,bool front)

//DLL_STRING_PTR_ARRAY_FUNC3(plateau_appearance_target_get_all_texture_themes,
//                           AppearanceTarget,
//                           handle->getAllTextureThemes(front).size(),
//                           const auto& str : handle->getAllTextureThemes(front),
//                           str,
//                           ,bool front)

LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_appearance_target_get_all_texture_themes(
        const AppearanceTarget* const handle,
        char** out_strs,
        bool front
        ){
        API_TRY {
            int i = 0;
            for (const auto &str: handle->getAllTextureThemes(front)) {
                // 文字列を out_strs[i] にコピーします。
                auto chars = str.c_str();
                auto len = (dll_str_size_t) (strlen(chars));
                strncpy(out_strs[i], chars, len);
                out_strs[i][len] = '\0'; // 最後はnull終端文字
                i++;
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

}