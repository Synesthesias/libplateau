#include <iostream>
#include <citygml/object.h>

#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

DLL_STRING_PTR_FUNC(plateau_object_get_id,
                  Object,
                  handle->getId())

DLL_PTR_FUNC(plateau_object_get_attributes_map,
             Object,
             AttributesMap,
             &handle->getAttributes())
}
