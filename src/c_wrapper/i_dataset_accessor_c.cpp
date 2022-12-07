#include "libplateau_c.h"
#include <plateau/dataset/i_dataset_accessor.h>

extern "C" {
    using namespace plateau::dataset;
    using namespace plateau::geometry;
    using namespace libplateau;


    DLL_3_ARG_FUNC(plateau_i_dataset_accessor_get_gml_files,
                   IDatasetAccessor* accessor,
                   PredefinedCityModelPackage package,
                   std::vector<GmlFile>* out_gml_files,
                   accessor->getGmlFiles(package, *out_gml_files))

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_i_dataset_accessor_get_mesh_codes(
            IDatasetAccessor* const dataset_accessor,
            std::vector<MeshCode>* const out_mesh_codes
    ) {
        API_TRY{
            const auto& mesh_codes = dataset_accessor->getMeshCodes();
            for (const auto& mesh_code : mesh_codes)
                out_mesh_codes->push_back(mesh_code);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }


//    DLL_VALUE_FUNC(plateau_i_dataset_accessor_get_max_lod,
//                   IDatasetAccessor,
//                   int,
//                   handle->getMaxLod(mesh_code, package), , MeshCode mesh_code, PredefinedCityModelPackage package)

    DLL_VALUE_FUNC(plateau_i_dataset_accessor_get_packages,
                   IDatasetAccessor,
                   PredefinedCityModelPackage,
                   handle->getPackages())
}
