#include "libplateau_c.h"
#include "city_model_c.h"
#include <plateau/geometry/geometry_utils.h>
using namespace libplateau;
using namespace plateau::geometry;

extern "C" {

DLL_VALUE_FUNC(plateau_geometry_utils_get_center_point,
               CityModelHandle,
               TVec3d,
               GeometryUtils::getCenterPoint(*handle->getCityModelPtr()))

}