#include "libplateau_c.h"
#include "city_model_c.h"
#include <plateau/polygon_mesh/polygon_mesh_utils.h>
using namespace libplateau;
using namespace plateau::polygonMesh;

extern "C" {

DLL_VALUE_FUNC(plateau_geometry_utils_get_center_point,
               CityModelHandle,
               TVec3d,
               PolygonMeshUtils::getCenterPoint(*handle->getCityModelPtr()))

}
