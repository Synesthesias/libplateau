#include "libplateau_c.h"
#include <plateau/network/client.h>

extern "C" {
using namespace plateau::network;
using namespace libplateau;

// TODO CREATE,DELETEのマクロをせっかく作ったので、他のものもそちらに置き換えたい
DLL_CREATE_FUNC(plateau_create_dataset_metadata_group,
                DatasetMetadataGroup)
DLL_DELETE_FUNC(plateau_delete_dataset_metadata_group,
                DatasetMetadataGroup)

DLL_STRING_PTR_FUNC(plateau_dataset_metadata_group_get_id,
                    DatasetMetadataGroup,
                    handle->id)

DLL_STRING_PTR_FUNC(plateau_dataset_metadata_group_get_title,
                    DatasetMetadataGroup,
                    handle->title)

DLL_PTR_FUNC(plateau_dataset_metadata_group_get_datasets,
             DatasetMetadataGroup,
             std::vector<DatasetMetadata>,
             &(handle->datasets))

}
