#include <plateau/geometry/geo_reference.h>
#include "polar_to_plane_cartesian.h"
#include "plateau/io/obj_writer.h"

namespace plateau::geometry {
    GeoReference::GeoReference(const TVec3d& reference_point, float unit_scale, CoordinateSystem coordinate_system,
                               int zone_id) :
            reference_point_(reference_point),
            coordinate_system_(coordinate_system),
            unit_scale_(unit_scale),
            zone_id_(zone_id) {}

    TVec3d GeoReference::project(const GeoCoordinate& point) const {
        double lat_lon[3] = {point.latitude, point.longitude, point.height};
        return project(lat_lon);
    }

    TVec3d GeoReference::project(const TVec3d& lat_lon) const {
        TVec3d point = lat_lon;
        PolarToPlaneCartesian().project(point, zone_id_);
        point = (point - reference_point_) / unit_scale_;

        switch (coordinate_system_) {
            case CoordinateSystem::ENU:
                return point;
            case CoordinateSystem::WUN:
                return {-point.x, point.z, point.y};
            case CoordinateSystem::NWU:
                return {point.y, -point.x, point.z};
            default:
                throw std::out_of_range("Invalid argument");
        }
    }

    void GeoReference::setReferencePoint(TVec3d point) {
        reference_point_ = point;
    }

    TVec3d GeoReference::getReferencePoint() const {
        return reference_point_;
    }

    int GeoReference::getZoneID() const {
        return zone_id_;
    }

    void GeoReference::setZoneID(int value) {
        zone_id_ = value;
    }

    GeoCoordinate GeoReference::unproject(const TVec3d& point) const {
        TVec3d lat_lon = point;
        PolarToPlaneCartesian().unproject(lat_lon, zone_id_);
        return GeoCoordinate(lat_lon.x, lat_lon.y, lat_lon.z);
    }
}
