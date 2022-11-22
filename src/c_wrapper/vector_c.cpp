#include "libplateau_c.h"
#include <vector>
#include <plateau/udx/gml_file.h>
extern "C"{
    using namespace libplateau;
    using namespace plateau::udx;

/**
 * vector<任意の型> を P/Invoke で扱うための5つのメソッドを生成するマクロです。
 * 5つのメソッドとは
 * 1: plateau_create_vector_FUNC_NAME (new)
 * 2: plateau_delete_vector_FUNC_NAME (delete),
 * 3: plateau_vector_FUNC_NAME_get_pointer (要素のポインタを取得),
 * 4: plateau_vector_FUNC_NAME_get_value (要素の値を取得)
 * 5: plateau_vector_FUNC_NAME_count (要素数を取得)
 * です。
 * なお通常は、 3か4のどちらか片方は利用しないはずです。
 * vectorの要素は実体で保持されます。
 * その実体が P/Invoke で直接に値渡しできる(簡単な構造体など)場合は 4 を利用、
 * できない場合は 3 を利用することを想定しています。
 */
#define PLATEAU_VECTOR(FUNC_NAME, VECTOR_ELEMENT_TYPE) \
LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_API plateau_create_vector_ ## FUNC_NAME ( \
    std::vector< VECTOR_ELEMENT_TYPE >** out_vector_ptr){ \
    *out_vector_ptr = new std::vector< VECTOR_ELEMENT_TYPE >(); \
    return APIResult::Success; \
    } \
     \
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_vector_ ## FUNC_NAME ( \
    std::vector< VECTOR_ELEMENT_TYPE >** vector_ptr \
    ){ \
        delete vector_ptr; \
        return APIResult::Success; \
    } \
    \
    DLL_PTR_FUNC_WITH_INDEX_CHECK (plateau_vector_ ## FUNC_NAME ## _get_pointer, \
        std::vector< VECTOR_ELEMENT_TYPE >, \
        VECTOR_ELEMENT_TYPE, \
        &(handle->at(index)), \
        index >= handle->size()) \
    \
    DLL_VALUE_FUNC_WITH_INDEX_CHECK (plateau_vector_ ## FUNC_NAME ## _get_value,   \
        std::vector< VECTOR_ELEMENT_TYPE >, \
        VECTOR_ELEMENT_TYPE, \
        handle->at(index), \
        index >= handle->size()) \
        \
    DLL_VALUE_FUNC (plateau_vector_ ## FUNC_NAME ## _count, \
        std::vector< VECTOR_ELEMENT_TYPE >, \
        int, \
        handle->size())


    PLATEAU_VECTOR(gml_file, GmlFile)
    PLATEAU_VECTOR(mesh_code, MeshCode)
}
