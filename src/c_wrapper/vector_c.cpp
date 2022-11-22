#include "libplateau_c.h"
#include <vector>
#include <plateau/udx/gml_file.h>
extern "C"{
    using namespace libplateau;
    using namespace plateau::udx;

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_vector_gml_file(
            std::vector<GmlFile>** out_vector_ptr
            ){
        API_TRY{
            *out_vector_ptr = new std::vector<GmlFile>();
            return APIResult::Success;
        }API_CATCH
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_vector_gml_file(
            std::vector<GmlFile>* vector_ptr
            ){
        delete vector_ptr;
        return APIResult::Success;
    }

    DLL_PTR_FUNC_WITH_INDEX_CHECK(plateau_vector_gml_file_get_gml_file,
                                  std::vector<GmlFile>,
                                  GmlFile,
                                  &(handle->at(index)),
                                  index >= handle->size())

    DLL_VALUE_FUNC(plateau_vector_gml_file_count,
                   std::vector<GmlFile>,
                   int,
                   handle->size())
}
