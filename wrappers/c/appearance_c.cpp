#include <citygml/appearance.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {
DLL_STRING_PTR_FUNC(plateau_appearance_get_type,
                    Appearance,
                    handle->getType())
}