#include <citygml/appearancetarget_c.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

DLL_STRING_PTR_ARRAY_FUNC3(plateau_appearance_target_get_all_texture_themes,
                           AppearanceTarget,
                           handle->getAllTextureThemes(front).size(),
                           const auto& str : handle->getAllTextureThemes(front),
                           str,
                           ,bool front)

}