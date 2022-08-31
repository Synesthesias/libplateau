#include <citygml/appearancetarget.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

DLL_STRING_VALUE_ARRAY_FUNC3(plateau_appearance_target_get_all_texture_theme_names,
                             AppearanceTarget,
                             handle->getAllTextureThemes(front).size(),
                             const auto &str: handle->getAllTextureThemes(front),
                             str,
                             ,bool front)

DLL_STRING_VALUE_ARRAY_FUNC3(plateau_appearance_target_get_all_material_theme_names,
                             AppearanceTarget,
                             handle->getAllMaterialThemes(front).size(),
                             const auto &str : handle->getAllMaterialThemes(front),
                             str,
                             ,bool front)

DLL_VALUE_FUNC(plateau_appearance_target_get_texture_target_definitions_count,
               AppearanceTarget,
               int,
               handle->getTextureTargetDefinitions().size())

DLL_PTR_FUNC_WITH_INDEX_CHECK(plateau_appearance_target_get_texture_target_definition_by_index,
             AppearanceTarget,
             TextureTargetDefinition,
             handle->getTextureTargetDefinitions().at(index),
             index >= handle->getTextureTargetDefinitions().size())

DLL_PTR_FUNC(plateau_appearance_target_get_material_target_definition_by_theme_name,
             AppearanceTarget,
             MaterialTargetDefinition,
             handle->getMaterialTargetDefinitionForTheme(std::string(theme_name), front).get();
             if(*out == nullptr) return APIResult::ErrorValueNotFound;,
             ,char* theme_name, bool front)

/// 引数 theme_chars に対応する TextureTarget がない場合は ErrorValueNotFound を返します。
DLL_PTR_FUNC(plateau_appearance_target_get_texture_target_definition_by_theme_name,
             AppearanceTarget,
             TextureTargetDefinition,
             handle->getTextureTargetDefinitionForTheme(std::string(theme_name), front).get();
             if(*out == nullptr) return APIResult::ErrorValueNotFound;,
             ,char* theme_name, bool front)
}
