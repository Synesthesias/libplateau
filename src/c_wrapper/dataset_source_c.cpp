#include "libplateau_c.h"
#include <plateau/dataset/dataset_source.h>
#include <filesystem>

extern "C" {
    using namespace libplateau;
    using namespace plateau::dataset;
    namespace fs = std::filesystem;

    DLL_2_ARG_FUNC(plateau_create_dataset_source_local,
                   DatasetSource** out_dataset_source,
                   const char* const source_path_utf8,
                   *out_dataset_source = new DatasetSource(DatasetSource::createLocal(source_path_utf8)) )

    DLL_3_ARG_FUNC(plateau_create_dataset_source_server,
                   DatasetSource** out_new_dataset_source,
                   const char* const dataset_id,
                   plateau::network::Client* client,
                   *out_new_dataset_source = new DatasetSource(DatasetSource::createServer(std::string(dataset_id), *client)))

    DLL_DELETE_FUNC(plateau_delete_dataset_source,
                    DatasetSource)

    /**
     * IDatasetAccessor の delete は DLL利用者の責任とします。
     */
    DLL_2_ARG_FUNC(plateau_dataset_source_get_accessor,
                   const DatasetSource* const dataset_source,
                   const IDatasetAccessor** const out_dataset_accessor_ptr,

                   auto accessor_ptr = dataset_source->getAccessor();
                   *out_dataset_accessor_ptr = accessor_ptr->clone();
    )
}
