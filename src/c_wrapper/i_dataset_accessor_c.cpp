#include "libplateau_c.h"
#include <plateau/dataset/i_dataset_accessor.h>
#include <vector>
#include <string>

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

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_i_dataset_accessor_get_grid_codes(
            IDatasetAccessor* const dataset_accessor,
            std::vector<GridCode*>* const out_grid_codes
    ) {
        API_TRY{
            const auto& grid_codes = dataset_accessor->getGridCodes();
            for (const auto& grid_code : grid_codes) {
                out_grid_codes->push_back(GridCode::createRaw(grid_code->get()));  // GridCode::createRawを使用
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

    DLL_3_ARG_FUNC(plateau_i_dataset_accessor_filter_by_grid_codes,
                   const IDatasetAccessor* const accessor,
                   const std::vector<GridCode*>* grid_codes,
                   IDatasetAccessor** out_dataset_accessor_ptr,
                   auto filtered = accessor->create();
                           accessor->filterByGridCodes(*grid_codes, *filtered);
                   *out_dataset_accessor_ptr = filtered;
    )

}
