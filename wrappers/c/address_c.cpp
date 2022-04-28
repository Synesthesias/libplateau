#include <citygml/address.h>
#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C"{


    DLL_STRING_PTR_FUNC2(plateau_address_get_country,
                      Address,
                      handle->country())



    DLL_CHAR_PTR_FUNC(plateau_address_get_locality,
                      Address,
                      handle->locality().c_str())

    DLL_VALUE_FUNC(plateau_address_get_locality_str_length,
                   Address,
                   int,
                   handle->locality().length() + 1)

    DLL_CHAR_PTR_FUNC(plateau_address_get_postal_code,
                      Address,
                      handle->postalCode().c_str())

    DLL_VALUE_FUNC(plateau_address_get_postal_code_str_length,
                   Address,
                   int,
                   handle->postalCode().length() + 1)

    DLL_CHAR_PTR_FUNC(plateau_address_get_thoroughfare_name,
                      Address,
                      handle->thoroughfareName().c_str())

    DLL_VALUE_FUNC(plateau_address_get_thoroughfare_name_str_length,
                   Address,
                   int,
                   handle->thoroughfareName().length() + 1)

    DLL_CHAR_PTR_FUNC(plateau_address_get_thoroughfare_number,
                      Address,
                      handle->thoroughfareNumber().c_str())

    DLL_VALUE_FUNC(plateau_address_get_thoroughfare_number_str_length,
                   Address,
                   int,
                   handle->thoroughfareNumber().length() + 1)
}