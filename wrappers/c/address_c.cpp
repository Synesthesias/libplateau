#include <citygml/address.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C"{

    DLL_STRING_PTR_FUNC2(plateau_address_get_country,
                      Address,
                      handle->country())

    DLL_STRING_PTR_FUNC2(plateau_address_get_locality,
                      Address,
                      handle->locality())

    DLL_STRING_PTR_FUNC2(plateau_address_get_postal_code,
                      Address,
                      handle->postalCode())

    DLL_STRING_PTR_FUNC2(plateau_address_get_thoroughfare_name,
                      Address,
                      handle->thoroughfareName())

    DLL_STRING_PTR_FUNC2(plateau_address_get_thoroughfare_number,
                      Address,
                      handle->thoroughfareNumber())
}