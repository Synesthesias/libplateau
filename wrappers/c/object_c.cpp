#include <iostream>
#include <obj_writer.h>

#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

    DLL_CHAR_PTR_FUNC(plateau_object_get_id,
                      Object,
                      handle->getId().c_str())



    DLL_PTR_FUNC(plateau_object_get_attributes_map,
                 Object,
                 AttributesMap,
                 &handle->getAttributes())

    
}