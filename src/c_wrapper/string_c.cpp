#include "libplateau_c.h"

extern "C"{

DLL_CREATE_FUNC(plateau_create_string,
                std::string)

DLL_DELETE_FUNC(plateau_delete_string,
                std::string)

DLL_2_ARG_FUNC(plateau_string_get_size,
               const std::string* const str,
               int* const out_size,
               *out_size = (int)str->size())

DLL_2_ARG_FUNC(plateau_string_get_char_ptr,
               const std::string* const str,
               const char** const out_char_ptr,
               *out_char_ptr = str->c_str())
}
