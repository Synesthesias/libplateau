#include "libplateau_c.h"
#include <plateau/udx/dataset_source.h>
#include <plateau/udx/dataset_accessor_p_invoke.h>

extern "C"{
    using namespace libplateau;
    using namespace plateau::udx;
    namespace fs = std::filesystem;

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_dataset_source_local(
            DatasetSource** out_dataset_source,
            const char* const source_path_utf8
            ){
        API_TRY{
            *out_dataset_source = new DatasetSource(fs::u8path(source_path_utf8));
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_dataset_source(
            DatasetSource* dataset_source
            ){
        delete dataset_source;
        return APIResult::Success;
    }

    /**
     * DatasetAccessorPInvoke の delete は DLL利用者の責任とします。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_dataset_source_get_accessor(
            DatasetSource* dataset_source,
            DatasetAccessorPInvoke** out_dataset_accessor_p_invoke
    ){
        API_TRY{
            auto accessor = dataset_source->getAccessor();
            auto accessor_p_invoke = new DatasetAccessorPInvoke(accessor);
            *out_dataset_accessor_p_invoke = accessor_p_invoke;
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
