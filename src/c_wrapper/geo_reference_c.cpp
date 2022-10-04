#include "libplateau_c.h"
#include <plateau/geometry/geo_reference.h>

using namespace libplateau;
using namespace plateau::geometry;
extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_geo_reference(
            GeoReference** out_geo_reference,
            TVec3d reference_point,
            float unit_scale,
            CoordinateSystem coordinate_system,
            int zone_id
            ) {
        API_TRY {
            *out_geo_reference = new GeoReference(zone_id, reference_point, unit_scale, coordinate_system);
            return APIResult::Success;
        } API_CATCH
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_geo_reference(
            GeoReference* geo_reference
            ){
        API_TRY{
            delete geo_reference;
            return APIResult::Success;
        }API_CATCH
        return APIResult::ErrorUnknown;
    }

DLL_VALUE_FUNC(plateau_geo_reference_project,
               GeoReference,
               TVec3d,
               handle->project(lat_lon),
               , GeoCoordinate lat_lon)

    DLL_VALUE_FUNC(plateau_geo_reference_unproject,
               GeoReference,
               GeoCoordinate,
               handle->unproject(point),
               , TVec3d point)
}
