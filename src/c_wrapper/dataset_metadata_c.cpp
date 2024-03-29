#include "libplateau_c.h"
#include <plateau/network/client.h>

extern "C" {
using namespace plateau::network;
using namespace libplateau;

DLL_CREATE_FUNC(plateau_create_dataset_metadata,
                DatasetMetadata)

DLL_DELETE_FUNC(plateau_delete_dataset_metadata,
                DatasetMetadata)

DLL_STRING_PTR_FUNC(plateau_dataset_metadata_get_id,
                    DatasetMetadata,
                    handle->id)

DLL_STRING_PTR_FUNC(plateau_dataset_metadata_get_title,
                    DatasetMetadata,
                    handle->title)

DLL_STRING_PTR_FUNC(plateau_dataset_metadata_get_description,
                    DatasetMetadata,
                    handle->description)

DLL_2_ARG_FUNC(plateau_dataset_metadata_get_feature_types,
               const DatasetMetadata* const meta_data,
               std::vector<std::string>* const feature_types,
               *feature_types = meta_data->feature_types)
}
