#include "libplateau_c.h"
#include "city_model_c.h"
#include <plateau/polygon_mesh/mesh_extractor.h>
#include <plateau/geometry/geo_coordinate.h>

using namespace libplateau;
using namespace plateau::polygonMesh;

extern "C"{

    /**
     * MeshExtractor::extract して結果を out_model に格納します。
     * out_model の delete はDLL利用者の責任です。
     * ただしModelをdeleteすると配下のNode,Meshにもアクセスできなくなるので注意してください。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_extract(
            const CityModelHandle* const city_model_handle,
            const MeshExtractOptions options,
            Model* const out_model){
        API_TRY{
            MeshExtractor::extract(*out_model, city_model_handle->getCityModel(), options);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }


    /**
     * MeshExtractor::extract して結果を out_model に格納します。
     * out_model の delete はDLL利用者の責任です。
     * ただしModelをdeleteすると配下のNode,Meshにもアクセスできなくなるので注意してください。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_extract_in_extents(
            const CityModelHandle* const city_model_handle,
            const MeshExtractOptions options,
            const std::vector<plateau::geometry::Extent> extents,
            Model* const out_model) {
        API_TRY{
            MeshExtractor::extractInExtents(*out_model, city_model_handle->getCityModel(), options, extents);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
