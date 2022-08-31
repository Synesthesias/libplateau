#include <citygml/texture.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {
DLL_STRING_PTR_FUNC(plateau_texture_get_url,
                    Texture,
                    handle->getUrl())

DLL_VALUE_FUNC(plateau_texture_get_wrap_mode,
               Texture,
               Texture::WrapMode,
               handle->getWrapMode())

}
