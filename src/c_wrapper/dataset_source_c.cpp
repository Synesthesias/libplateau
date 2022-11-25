#include "libplateau_c.h"
#include <plateau/dataset/dataset_source.h>

extern "C" {
    using namespace libplateau;
    using namespace plateau::dataset;
    namespace fs = std::filesystem;

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_dataset_source_local(
            DatasetSource** out_dataset_source,
            const char* const source_path_utf8
    ) {
        API_TRY{
                *out_dataset_source = new DatasetSource(fs::u8path(source_path_utf8));
                return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_DELETE_FUNC(plateau_delete_dataset_source,
                    DatasetSource)

    /**
     * DatasetAccessorPInvoke の delete は DLL利用者の責任とします。
     */
DLL_1_ARG_FUNC(plateau_dataset_source_get_accessor,
               DatasetSource,
               IDatasetAccessor*,
               *arg = handle->getAccessor().get())
}
