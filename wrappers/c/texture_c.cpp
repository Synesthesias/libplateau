#include <citygml/texture_c.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {
DLL_STRING_PTR_FUNC(plateau_texture_get_url,
                    Texture,
                    handle->getUrl())

}