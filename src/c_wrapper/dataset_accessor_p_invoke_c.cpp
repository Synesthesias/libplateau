#include "libplateau_c.h"
#include "plateau/geometry/geo_coordinate.h"
#include <plateau/udx/i_dataset_accessor.h>
#include <plateau/udx/dataset_accessor_p_invoke.h>
extern "C" {
    using namespace plateau::udx;
    using namespace plateau::geometry;
    using namespace libplateau;
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_dataset_accessor_p_invoke(
            IDatasetAccessor* accessor,
            DatasetAccessorPInvoke** out_dataset_accessor_p_invoke
            ){
        API_TRY{
            *out_dataset_accessor_p_invoke = new DatasetAccessorPInvoke(std::shared_ptr<IDatasetAccessor>(accessor));
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_dataset_accessor_p_invoke(
            IDatasetAccessor* accessor
            ){
        delete accessor;
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_dataset_accessor_p_invoke_get_gml_files(
            DatasetAccessorPInvoke* accessor,
            Extent extent,
            PredefinedCityModelPackage package
            ){
        API_TRY{
            accessor->getGmlFiles(extent, package);
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }


    DLL_PTR_FUNC_WITH_INDEX_CHECK(plateau_dataset_accessor_p_invoke_result_of_get_gml_files,
                                  DatasetAccessorPInvoke,
                                  GmlFileInfo,
                                  handle->resultOfGetGmlFiles(index),
                                  index > (handle->resultOfGetGmlFilesCount()))

  DLL_VALUE_FUNC(plateau_dataset_accessor_p_invoke_result_of_get_gml_files_count,
                 DatasetAccessorPInvoke,
                 int,
                 handle->resultOfGetGmlFilesCount())

LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_dataset_accessor_p_invoke_get_mesh_codes(
        DatasetAccessorPInvoke* accessor
){
    API_TRY{
        accessor->getMeshCodes();
        return APIResult::Success;
    }API_CATCH;
    return APIResult::ErrorUnknown;
}


DLL_VALUE_FUNC_WITH_INDEX_CHECK(plateau_dataset_accessor_p_invoke_result_of_get_mesh_codes,
                              DatasetAccessorPInvoke,
                              MeshCode,
                              handle->resultOfGetMeshCodes(index),
                              index > (handle->resultOfGetMeshCodesCount()))

DLL_VALUE_FUNC(plateau_dataset_accessor_p_invoke_result_of_get_mesh_codes_count,
               DatasetAccessorPInvoke,
               int,
               handle->resultOfGetMeshCodesCount())
}
