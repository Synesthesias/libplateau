#include <citygml/appearancetarget.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

DLL_STRING_VALUE_ARRAY_FUNC3(plateau_appearance_target_get_all_texture_themes,
                             AppearanceTarget,
                             handle->getAllTextureThemes(front).size(),
                             const auto &str: handle->getAllTextureThemes(front),
                             str,
                             ,bool front)



// TODO ここの処理は attributesmap_c.cpp の get_attribute_value と類似しているのでマクロ化できないか検討します。
/// 引数 theme_chars に対応する TextureTarget がない場合は ErrorValueNotFound を返します。
LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_appearance_target_get_texture_target_definition_for_theme(
        const AppearanceTarget* const handle,
        const char* const theme_chars,
        const TextureTargetDefinition** const out_texture_target_definition,
        bool front
        ){
    API_TRY{
        const auto theme_str = std::string(theme_chars);
        auto tex_target_ptr = handle->getTextureTargetDefinitionForTheme(theme_str, front).get();
        if(tex_target_ptr == nullptr){
            return APIResult::ErrorValueNotFound;
        }
        *out_texture_target_definition = tex_target_ptr;
        return APIResult::Success;
    }
    API_CATCH;
    return APIResult::ErrorUnknown;
}

}