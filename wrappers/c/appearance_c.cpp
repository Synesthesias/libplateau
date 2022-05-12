#include <citygml/appearance.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {
DLL_STRING_PTR_FUNC(plateau_appearance_get_type,
                    Appearance,
                    handle->getType())

DLL_VALUE_FUNC(plateau_appearance_get_is_front,
               Appearance,
               bool,
               handle->getIsFront())
}