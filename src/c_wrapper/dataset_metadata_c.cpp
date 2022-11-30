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

DLL_VALUE_FUNC(plateau_dataset_metadata_get_max_lod,
               DatasetMetadata,
               int,
               handle->max_lod)

               // TODO feature_typesを取得するP/Invoke
}
