#include "libplateau_c.h"
#include "plateau/geometry/geo_coordinate.h"
#include <plateau/dataset/server_dataset_accessor.h>

extern "C" {
using namespace plateau::dataset;
using namespace plateau::network;
using namespace plateau::geometry;
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

DLL_1_ARG_FUNC(plateau_server_dataset_accessor_set_dataset_id,
               ServerDatasetAccessor,
               const char,
               handle->setDatasetID(arg))

DLL_1_ARG_FUNC(plateau_server_dataset_accessor_get_mesh_codes,
               ServerDatasetAccessor,
               std::vector<MeshCode>,
               handle->getMeshCodes(*arg))

LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_server_dataset_accessor_get_gml_files(
       ServerDatasetAccessor* const accessor,
       std::vector<GmlFile>* out_gml_files,
       Extent extent,
       PredefinedCityModelPackage package
) {
    API_TRY{
        accessor->getGmlFiles(extent, package, *out_gml_files);
        return APIResult::Success;
    }API_CATCH
    return APIResult::ErrorUnknown;
}

DLL_1_ARG_FUNC(plateau_server_dataset_accessor_get_packages,
               ServerDatasetAccessor,
               PredefinedCityModelPackage,
               *arg = handle->getPackages())
}
