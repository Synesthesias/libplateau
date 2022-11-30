#include "libplateau_c.h"

extern "C"{
DLL_2_ARG_FUNC(plateau_string_get_size,
               const std::string* const, // str_ptr
               int* const, // out_size
               *arg_2 = (int)arg_1->size())

DLL_2_ARG_FUNC(plateau_string_get_char_ptr,
               const std::string* const, // str_ptr
               const char** const, // out_char_ptr
               *arg_2 = arg_1->c_str())
}
