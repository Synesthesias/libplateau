#include "libplateau_c.h"
#include <plateau/dataset/i_dataset_accessor.h>

extern "C" {
    using namespace plateau::dataset;
    using namespace plateau::geometry;
    using namespace libplateau;

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_i_dataset_accessor_get_gml_files(
            IDatasetAccessor* accessor,
            Extent extent,
            PredefinedCityModelPackage package,
            std::vector<GmlFile>* out_gml_files
    ) {
        API_TRY {
            accessor->getGmlFiles(extent, package, *out_gml_files);
            return APIResult::Success;
        } API_CATCH
        return APIResult::ErrorUnknown;
    }

    DLL_1_ARG_FUNC(plateau_i_dataset_accessor_get_mesh_codes,
                   IDatasetAccessor,
                   std::vector<MeshCode>,
                   handle->getMeshCodes(*arg))

    DLL_VALUE_FUNC(plateau_i_dataset_accessor_get_max_lod,
                   IDatasetAccessor,
                   int,
                   handle->getMaxLod(mesh_code, package), , MeshCode mesh_code, PredefinedCityModelPackage package)

    DLL_VALUE_FUNC(plateau_i_dataset_accessor_get_packages,
                   IDatasetAccessor,
                   PredefinedCityModelPackage,
                   handle->getPackages())
}
