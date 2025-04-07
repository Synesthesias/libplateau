#include "libplateau_c.h"
#include <plateau/dataset/i_dataset_accessor.h>

extern "C" {
    using namespace plateau::dataset;
    using namespace plateau::geometry;
    using namespace libplateau;

    DLL_DELETE_FUNC(plateau_delete_i_dataset_accessor,
                    IDatasetAccessor)

    DLL_3_ARG_FUNC(plateau_i_dataset_accessor_get_gml_files,
                   IDatasetAccessor* accessor,
                   PredefinedCityModelPackage package,
                   std::vector<GmlFile>* out_gml_files,
                   accessor->getGmlFiles(package, *out_gml_files))

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_i_dataset_accessor_get_mesh_codes(
            IDatasetAccessor* const dataset_accessor,
            std::vector<std::string>* const out_grid_codes
    ) {
        API_TRY{
            const auto& grid_codes = dataset_accessor->getGridCodes();
            for (const auto& grid_code : grid_codes) {
                out_grid_codes->push_back(grid_code->get());
            }
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_VALUE_FUNC(plateau_i_dataset_accessor_get_packages,
                   IDatasetAccessor,
                   PredefinedCityModelPackage,
                   handle->getPackages())

    DLL_3_ARG_FUNC(plateau_i_dataset_accessor_calculate_center_point,
                   IDatasetAccessor* const accessor,
                   const GeoReference* const geo_reference,
                   TVec3d* const out_center_point,
                   *out_center_point = accessor->calculateCenterPoint(*geo_reference))

    DLL_3_ARG_FUNC(plateau_i_dataset_accessor_filter_by_mesh_codes,
                   const IDatasetAccessor* const accessor,
                   const std::vector<MeshCode>* mesh_codes,
                   IDatasetAccessor** out_dataset_accessor_ptr,
                   auto filtered = accessor->create();
                   accessor->filterByMeshCodes(*mesh_codes, *filtered);
                   *out_dataset_accessor_ptr = filtered;
    )

}
