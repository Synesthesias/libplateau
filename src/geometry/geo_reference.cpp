#include <plateau/geometry/geo_reference.h>
#include "../io/polar_to_plane_cartesian.h"
#include "plateau/io/obj_writer.h"

namespace plateau::geometry{
    GeoReference::GeoReference(const TVec3d& reference_point, float unit_scale, CoordinateSystem coordinate_system,
                               int zone_id) :
        reference_point_(reference_point),
        coordinate_system_(coordinate_system),
        unit_scale_(unit_scale),
        zone_id_(zone_id){}

    TVec3d GeoReference::project(const GeoCoordinate& point) const {
        auto lat_lon = TVec3d(point.latitude, point.longitude, point.height);
        polar_to_plane_cartesian().convert(lat_lon);
        // TODO ObjWriterへの依存はやめる
        auto xyz = ObjWriter::convertPosition(lat_lon, reference_point_, coordinate_system_, unit_scale_);
        return xyz;
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

    void GeoReference::setZoneID(int value){
        zone_id_ = value;
    }
}
