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

    DLL_2_ARG_FUNC(plateau_create_dataset_source_server,
                   DatasetSource**, // out_pointer_of_new_instance
                   const char* const, // dataset_id
                   *arg_1 = new DatasetSource(std::string(arg_2)))

    DLL_DELETE_FUNC(plateau_delete_dataset_source,
                    DatasetSource)

    /**
     * DatasetAccessorPInvoke の delete は DLL利用者の責任とします。
     */
    DLL_2_ARG_FUNC(plateau_dataset_source_get_accessor,
                   const DatasetSource* const,
                   const IDatasetAccessor** const,
                   *arg_2 = arg_1->getAccessor().get())
}
