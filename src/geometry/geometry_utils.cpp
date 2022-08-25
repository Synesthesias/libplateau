#include <plateau/geometry/geometry_utils.h>
#include "plateau/io/polar_to_plane_cartesian.h"

using namespace plateau::geometry;

TVec3d GeometryUtils::getCenterPoint(const CityModel &cityModel) {
    auto& envelope = cityModel.getEnvelope();
    if(!envelope.validBounds()){
        return TVec3d{0,0,0};
    }
    auto cityCenter = (envelope.getLowerBound() + envelope.getUpperBound()) / 2.0;
    polar_to_plane_cartesian().convert(cityCenter);
    cityCenter.z = 0.0;
    return cityCenter;
}
