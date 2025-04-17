#include "libplateau_c.h"
#include "city_model_c.h"
#include <plateau/polygon_mesh/polygon_mesh_utils.h>
#include <plateau/geometry/geo_coordinate.h>
using namespace libplateau;
using namespace plateau::polygonMesh;
using namespace plateau::geometry;

extern "C" {

DLL_VALUE_FUNC(plateau_geometry_utils_get_center_point,
               CityModelHandle,
               TVec3d,
               PolygonMeshUtils::getCenterPoint(*handle->getCityModelPtr(), coordinate_zone_id),
               ,int coordinate_zone_id)

LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_geometry_utils_is_polar_coordinate_system(
    double epsg,
    bool* out
) {
API_TRY{
    *out = CoordinateReferenceFactory::IsPolarCoordinateSystem(epsg);
    return APIResult::Success;
} API_CATCH
    return APIResult::ErrorUnknown;
}

}
