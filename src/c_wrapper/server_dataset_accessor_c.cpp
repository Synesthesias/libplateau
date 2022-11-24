#include "libplateau_c.h"
#include <plateau/dataset/server_dataset_accessor.h>

extern "C" {
using namespace plateau::dataset;
using namespace plateau::network;
using namespace libplateau;

DLL_CREATE_FUNC(plateau_create_server_dataset_accessor,
                ServerDatasetAccessor)

DLL_DELETE_FUNC(plateau_delete_server_dataset_accessor,
                ServerDatasetAccessor)


// TODO DLL_1_ARG_FUNC というマクロをせっかく作ったので、他の所で同マクロに置き換え可能な箇所を置き換えたい
DLL_1_ARG_FUNC(plateau_server_dataset_accessor_get_dataset_metadata_group,
               ServerDatasetAccessor,
               std::vector<DatasetMetadataGroup>,
               handle->getDatasetMetadataGroup(*arg))
}
