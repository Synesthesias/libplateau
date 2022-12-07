#include "libplateau_c.h"
#include "plateau/geometry/geo_coordinate.h"
#include <plateau/dataset/server_dataset_accessor.h>

extern "C" {
using namespace plateau::dataset;
using namespace plateau::network;
using namespace plateau::geometry;
using namespace libplateau;


DLL_2_ARG_FUNC(plateau_create_server_dataset_accessor,
               ServerDatasetAccessor** out_ptr,
               const char* const dataset_id,
               *out_ptr = new ServerDatasetAccessor(dataset_id))

DLL_DELETE_FUNC(plateau_delete_server_dataset_accessor,
                ServerDatasetAccessor)

DLL_2_ARG_FUNC(plateau_server_dataset_accessor_get_dataset_metadata_group,
               const ServerDatasetAccessor* const server_dataset_accessor,
               std::vector<DatasetMetadataGroup>* const out_metadata_groups,
               server_dataset_accessor->getDatasetMetadataGroup(*out_metadata_groups))

DLL_2_ARG_FUNC(plateau_server_dataset_accessor_set_dataset_id,
               ServerDatasetAccessor* const server_dataset_accessor,
               const char* const dataset_id,
               server_dataset_accessor->setDatasetID(dataset_id))

DLL_2_ARG_FUNC(plateau_server_dataset_accessor_get_mesh_codes,
               ServerDatasetAccessor* const server_dataset_accessor,
               std::vector<MeshCode>* const out_mesh_codes,
               server_dataset_accessor->getMeshCodes(*out_mesh_codes))

LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_server_dataset_accessor_get_gml_files(
        ServerDatasetAccessor* const accessor,
        std::vector<GmlFile>* out_gml_files,
        Extent extent,
        PredefinedCityModelPackage package
) {
    API_TRY {
        accessor->getGmlFiles(extent, package, *out_gml_files);
        return APIResult::Success;
    } API_CATCH
    return APIResult::ErrorUnknown;
}

DLL_2_ARG_FUNC(plateau_server_dataset_accessor_get_packages,
               ServerDatasetAccessor* const server_dataset_accessor,
               PredefinedCityModelPackage* const out_packages,
               *out_packages = server_dataset_accessor->getPackages())

DLL_4_ARG_FUNC(plateau_server_dataset_accessor_get_max_lod,
               ServerDatasetAccessor* const,
               MeshCode,
               PredefinedCityModelPackage,
               int* const, // out_max_lod
               *arg_4 = arg_1->getMaxLod(arg_2, arg_3))
}
