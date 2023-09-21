#include <plateau/geometry/geo_reference.h>
#include "polar_to_plane_cartesian.h"
#include "plateau/mesh_writer/obj_writer.h"

namespace plateau::geometry {
    GeoReference::GeoReference(int coordinate_zone_id, const TVec3d& reference_point, float unit_scale,
                               CoordinateSystem coordinate_system) :
        reference_point_(reference_point),
        coordinate_system_(coordinate_system),
        unit_scale_(unit_scale),
        zone_id_(coordinate_zone_id) {
    }

    TVec3d GeoReference::project(const GeoCoordinate& point) const {
        double lat_lon[3] = { point.latitude, point.longitude, point.height };
        return project(lat_lon);
    }

    TVec3d GeoReference::project(const TVec3d& lat_lon) const {
        TVec3d point = lat_lon;
        PolarToPlaneCartesian().project(point, zone_id_);
        TVec3 converted_point = convertAxisFromENUTo(coordinate_system_, point);
        converted_point = converted_point / unit_scale_ - reference_point_;
        return converted_point;
    }

    TVec3d GeoReference::convertAxisToENU(const TVec3d& vertex) const {
        return convertAxisToENU(getCoordinateSystem(), vertex);
    }

    TVec3d GeoReference::projectWithoutAxisConvert(const TVec3d& lat_lon) const {
        // 前提として、座標軸は 変換前 ENU → 変換後 ENU であるとします。
        // そのため reference_point_ の代わりに reference_point_ を ENU に変換した値が利用されます。
        TVec3d point = lat_lon;
        PolarToPlaneCartesian().project(point, zone_id_);
        TVec3 converted_point = point / unit_scale_ - convertAxisToENU(coordinate_system_, reference_point_);
        return converted_point;
    }

    TVec3d GeoReference::convertAxisFromENUTo(CoordinateSystem axis, const TVec3d& vertex) {
        switch (axis) {
        case CoordinateSystem::ENU:
            return vertex; // 変換なし
        case CoordinateSystem::WUN:
            return { -vertex.x, vertex.z, vertex.y };
        case CoordinateSystem::ESU:
            return { vertex.x, -vertex.y, vertex.z };
        case CoordinateSystem::EUN:
            return { vertex.x, vertex.z, vertex.y };
        default:
            throw std::out_of_range("Invalid argument");
        }
    }

    TVec3d GeoReference::convertAxisToENU(CoordinateSystem axis, const TVec3d& vertex) {
        switch (axis) {
        case CoordinateSystem::ENU:
            // 変換なし
            return vertex;
        case CoordinateSystem::WUN:
            // WUN → ENU の式は 逆変換 ENU → WUN と同じです。
            return { -vertex.x, vertex.z, vertex.y };
        case CoordinateSystem::ESU:
            // EUN → ESU の式は 逆変換 ESU → EUN と同じです。
            return { vertex.x, -vertex.y, vertex.z };
        case CoordinateSystem::EUN:
            // EUN → ENU の式は 逆変換 ENU → EUN と同じです。
            return { vertex.x, vertex.z, vertex.y };
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

    float GeoReference::getUnitScale() const {
        return unit_scale_;
    }

    CoordinateSystem GeoReference::getCoordinateSystem() const {
        return coordinate_system_;
    }

    GeoCoordinate GeoReference::unproject(const TVec3d& point) const {
        TVec3d before_convert_lat_lon = (point + reference_point_) * unit_scale_;
        TVec3d lat_lon = convertAxisToENU(coordinate_system_, before_convert_lat_lon);
        PolarToPlaneCartesian().unproject(lat_lon, zone_id_);
        return GeoCoordinate(lat_lon.x, lat_lon.y, lat_lon.z);
    }
}
