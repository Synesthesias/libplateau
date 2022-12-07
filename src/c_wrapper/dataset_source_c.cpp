#include "libplateau_c.h"
#include <plateau/dataset/dataset_source.h>

extern "C" {
    using namespace libplateau;
    using namespace plateau::dataset;
    namespace fs = std::filesystem;

    DLL_2_ARG_FUNC(plateau_create_dataset_source_local,
                   DatasetSource** out_dataset_source,
                   const char* const source_path_utf8,
                   *out_dataset_source = new DatasetSource(DatasetSource::createLocal(fs::u8path(source_path_utf8))); )

    DLL_2_ARG_FUNC(plateau_create_dataset_source_server,
                   DatasetSource** out_pointer_of_new_instance,
                   const char* const dataset_id,
                   *out_pointer_of_new_instance = new DatasetSource(DatasetSource::createServer(std::string(dataset_id))))

    DLL_DELETE_FUNC(plateau_delete_dataset_source,
                    DatasetSource)

    /**
     * DatasetAccessorPInvoke の delete は DLL利用者の責任とします。
     */
    DLL_2_ARG_FUNC(plateau_dataset_source_get_accessor,
                   const DatasetSource* const dataset_source,
                   const IDatasetAccessor** const out_dataset_accessor_ptr,
                   *out_dataset_accessor_ptr = dataset_source->getAccessor().get())
}
