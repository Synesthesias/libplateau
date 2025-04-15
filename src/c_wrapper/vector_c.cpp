#include <vector>

#include "libplateau_c.h"

#include <plateau/network/client.h>
#include <plateau/dataset/gml_file.h>
#include <plateau/polygon_mesh/city_object_list.h>
#include <plateau/geometry/geo_coordinate.h>

extern "C"{
    using namespace libplateau;
    using namespace plateau::dataset;
    using namespace plateau::network;
    using namespace plateau::polygonMesh;
    using namespace plateau::geometry;

/**
 * vector<任意の型> を P/Invoke で扱うための7つのメソッドを生成するマクロです。
 * そのメソッドとは
 * 1: plateau_create_vector_FUNC_NAME (new)
 * 2: plateau_delete_vector_FUNC_NAME (delete),
 * 3: plateau_vector_FUNC_NAME_get_pointer (要素のポインタを取得),
 * 4: plateau_vector_FUNC_NAME_get_value (要素の値を取得)
 * 5: plateau_vector_FUNC_NAME_count (要素数を取得)
 * 6: plateau_vector_FUNC_NAME_push_back_value (要素を追加, 値渡し)
 * 7: plateau_vector_FUNC_NAME_push_back_copy_of_ref (要素を追加, ポインタ渡しでポインタの参照先のコピーを追加)
 * です。
 * なお通常は、 3か4のどちらか片方は利用しないはずです。
 * vectorの要素は実体で保持されます。
 * その実体が P/Invoke で直接に値渡しできる(簡単な構造体など)場合は 4 を利用、
 * できない場合は 3 を利用することを想定しています。
 */
#define PLATEAU_VECTOR_GET_BY_PTR(FUNC_NAME, VECTOR_ELEMENT_TYPE) \
    PLATEAU_VECTOR_BASE(FUNC_NAME, VECTOR_ELEMENT_TYPE)           \
    GET_BY_PTR(FUNC_NAME, VECTOR_ELEMENT_TYPE)

#define PLATEAU_VECTOR_GET_BY_VALUE(FUNC_NAME, VECTOR_ELEMENT_TYPE) \
    PLATEAU_VECTOR_BASE(FUNC_NAME, VECTOR_ELEMENT_TYPE)           \
    GET_BY_VALUE(FUNC_NAME, VECTOR_ELEMENT_TYPE)

#define PLATEAU_VECTOR_BASE(FUNC_NAME, VECTOR_ELEMENT_TYPE) \
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
    DLL_VALUE_FUNC (plateau_vector_ ## FUNC_NAME ## _count, \
        std::vector< VECTOR_ELEMENT_TYPE >, \
        int, \
        handle->size()) \
    \
    DLL_2_ARG_FUNC (plateau_vector_ ## FUNC_NAME ## _push_back_value,\
        std::vector< VECTOR_ELEMENT_TYPE >* const vector, \
        VECTOR_ELEMENT_TYPE element, \
        vector->push_back(element))                    \
                                                       \
    DLL_2_ARG_FUNC(plateau_vector_ ## FUNC_NAME ## _push_back_copy_of_ref, \
        std::vector< VECTOR_ELEMENT_TYPE >* const vector, \
        VECTOR_ELEMENT_TYPE* element_ptr, /*NOLINT*/ \
        vector->push_back(*element_ptr))


#define GET_BY_PTR(FUNC_NAME, VECTOR_ELEMENT_TYPE) \
    DLL_PTR_FUNC_WITH_INDEX_CHECK (plateau_vector_ ## FUNC_NAME ## _get_pointer, \
            std::vector< VECTOR_ELEMENT_TYPE >, \
            VECTOR_ELEMENT_TYPE, \
            &(handle->at(index)), \
            index >= handle->size())

#define GET_BY_VALUE(FUNC_NAME, VECTOR_ELEMENT_TYPE) \
    DLL_VALUE_FUNC_WITH_INDEX_CHECK (plateau_vector_ ## FUNC_NAME ## _get_value,   \
        std::vector< VECTOR_ELEMENT_TYPE >, \
        VECTOR_ELEMENT_TYPE, \
        handle->at(index), \
        index >= handle->size())

/**
 * vector<ポインタ型> の要素をdeleteしてからvectorをクリアするための関数を生成するマクロです。
 * これはvectorがポインタを含む場合（例：vector<GridCode*>）に使用します。
 * 生成される関数は plateau_cleanup_vector_FUNC_NAME という名前になります。
 */
#define CLEANUP(FUNC_NAME, VECTOR_ELEMENT_TYPE) \
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_cleanup_vector_ ## FUNC_NAME ( \
        std::vector< VECTOR_ELEMENT_TYPE >* vector_ptr \
    ){ \
        if (vector_ptr == nullptr) return APIResult::ErrorInvalidArgument; \
        for (auto* element : *vector_ptr) { \
            delete element; \
        } \
        vector_ptr->clear(); \
        return APIResult::Success; \
    }

    PLATEAU_VECTOR_GET_BY_PTR(gml_file, GmlFile)
    PLATEAU_VECTOR_GET_BY_VALUE(grid_code, GridCode*)
    CLEANUP(grid_code, GridCode*)
    PLATEAU_VECTOR_GET_BY_PTR(dataset_metadata_group, DatasetMetadataGroup)
    PLATEAU_VECTOR_GET_BY_PTR(dataset_metadata, DatasetMetadata)
    PLATEAU_VECTOR_GET_BY_PTR(string, std::string)
    PLATEAU_VECTOR_GET_BY_VALUE(city_object_index, CityObjectIndex)
    PLATEAU_VECTOR_GET_BY_VALUE(extent, Extent)

}
