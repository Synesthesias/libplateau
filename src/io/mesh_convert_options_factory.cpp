#include <citygml/citymodel.h>
#include <plateau/io/mesh_convert_options_factory.h>

#include "polar_to_plane_cartesian.h"
#include "plateau/geometry/geo_reference.h"

void MeshConvertOptionsFactory::setValidReferencePoint(MeshConvertOptions& options,
                                                       const citygml::CityModel& city_model) {
    TVec3d lower_bound = city_model.getEnvelope().getLowerBound();
    TVec3d upper_bound = city_model.getEnvelope().getUpperBound();

    auto geo_reference = GeoReference(options.coordinate_zone_id);
    lower_bound = geo_reference.project(lower_bound);
    upper_bound = geo_reference.project(upper_bound);

    options.reference_point.x = (lower_bound.x + upper_bound.x) / 2.0;
    options.reference_point.y = (lower_bound.y + upper_bound.y) / 2.0;
    options.reference_point.z = lower_bound.z;
}
