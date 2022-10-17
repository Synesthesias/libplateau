#include <plateau/geometry/geo_reference.h>
#include "polar_to_plane_cartesian.h"
#include "plateau/mesh_writer/obj_writer.h"

namespace plateau::geometry {
    GeoReference::GeoReference(int coordinate_zone_id, const TVec3d& reference_point, float unit_scale,
                               CoordinateSystem coordinate_system) :
            reference_point_(reference_point),
            coordinate_system_(coordinate_system),
            unit_scale_(unit_scale),
            zone_id_(coordinate_zone_id) {}

    TVec3d GeoReference::project(const GeoCoordinate& point) const {
        double lat_lon[3] = {point.latitude, point.longitude, point.height};
        return project(lat_lon);
    }

    TVec3d GeoReference::project(const TVec3d& lat_lon) const {
        TVec3d point = lat_lon;
        PolarToPlaneCartesian().project(point, zone_id_);
        TVec3 converted_point = point;
        switch (coordinate_system_) {
            case CoordinateSystem::ENU:
                break; // 変換なし
            case CoordinateSystem::WUN:
                converted_point = {-point.x, point.z, point.y};
                break;
            case CoordinateSystem::EUN:
                converted_point = {point.x, point.z, point.y};
                break;
            case CoordinateSystem::NWU:
                converted_point = {point.y, -point.x, point.z};
                break;
            default:
                throw std::out_of_range("Invalid argument");
        }
        converted_point = converted_point / unit_scale_ - reference_point_;
        return converted_point;
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
        TVec3d before_convert_lat_lon = (point + reference_point_) * unit_scale_;
        TVec3d lat_lon;
        switch (coordinate_system_) {
        case CoordinateSystem::ENU:
            lat_lon = before_convert_lat_lon;
            break;
        case CoordinateSystem::WUN:
            lat_lon.x = -before_convert_lat_lon.x;
            lat_lon.y = before_convert_lat_lon.z;
            lat_lon.z = before_convert_lat_lon.y;
            break;
        case CoordinateSystem::NWU:
            lat_lon.x = -before_convert_lat_lon.y;
            lat_lon.y = before_convert_lat_lon.x;
            lat_lon.z = before_convert_lat_lon.z;
            break;
        case CoordinateSystem::EUN:
            lat_lon.x = before_convert_lat_lon.x;
            lat_lon.y = before_convert_lat_lon.z;
            lat_lon.z = before_convert_lat_lon.y;
            break;
        default:
            throw std::out_of_range("Invalid argument");
        }

        PolarToPlaneCartesian().unproject(lat_lon, zone_id_);
        return GeoCoordinate(lat_lon.x, lat_lon.y, lat_lon.z);
    }
}
