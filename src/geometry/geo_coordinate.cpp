#include <plateau/geometry/geo_coordinate.h>
#include <plateau/polygon_mesh/polygon_mesh_utils.h>
#include <cmath>

namespace plateau::geometry {
    using namespace citygml;
    using namespace polygonMesh;

    GeoCoordinate GeoCoordinate::operator+(GeoCoordinate op) const {
        return {
                latitude + op.latitude,
                longitude + op.longitude,
                height + op.height
                };
    }

    GeoCoordinate GeoCoordinate::operator*(double op) const {
        return {
            latitude * op,
            longitude * op,
            height * op
        };
    }

    GeoCoordinate GeoCoordinate::operator-(GeoCoordinate op) const {
        return GeoCoordinate(*this) + (op * -1);
    }

    GeoCoordinate GeoCoordinate::operator/(GeoCoordinate op) const {
        constexpr double double_max = std::numeric_limits<double>::infinity();
        const auto lat = op.latitude == 0 ? double_max : latitude / op.latitude;
        const auto lon = op.longitude == 0 ? double_max : longitude / op.longitude;
        const auto h = op.height == 0 ? double_max : height / op.height;
        return {lat, lon, h};
    }

    bool Extent::contains(GeoCoordinate point, bool ignore_height) const {
        const auto is_contain = min.latitude <= point.latitude &&
            max.latitude >= point.latitude &&
            min.longitude <= point.longitude &&
            max.longitude >= point.longitude;

        if (ignore_height)
            return is_contain;

        return is_contain &&
               min.height <= point.height &&
               max.height >= point.height;
    }

    bool Extent::contains(TVec3d point, bool ignore_height) const {
        return contains(GeoCoordinate(point.x, point.y, point.z), ignore_height);
    }

    bool Extent::contains(const CityObject& city_obj, bool ignore_height) const{
        try{
            auto pos = PolygonMeshUtils::cityObjPos(city_obj);
            return contains(pos, ignore_height);
        }catch(std::invalid_argument& e){
            // 位置不明は false 扱いとします。
            return false;
        }
    }

    bool Extent::intersects2D(const Extent& other) const {
        const auto center_1 = centerPoint();
        const auto center_2 = other.centerPoint();
        const auto lat_diff = std::abs(center_1.latitude - center_2.latitude);
        const auto lon_diff = std::abs(center_1.longitude - center_2.longitude);
        const auto lat_size_1 = max.latitude - min.latitude;
        const auto lat_size_2 = other.max.latitude - other.min.latitude;
        const auto lon_size_1 = max.longitude - min.longitude;
        const auto lon_size_2 = other.max.longitude - other.min.longitude;
        return
                lat_diff < (lat_size_1 + lat_size_2) * 0.5 &&
                lon_diff < (lon_size_1 + lon_size_2) * 0.5;
    }

    GeoCoordinate Extent::centerPoint() const {
        auto ret = GeoCoordinate(
                (min.latitude + max.latitude) * 0.5,
                (min.longitude + max.longitude) * 0.5,
                (min.height + max.height) * 0.5
                );
        return ret;
    }

    TVec2f Extent::uvAt(GeoCoordinate coord) const {
        const auto len = max - min;
        const auto diff = coord - min;
        const auto ratio = diff / len;
        return {(float)ratio.longitude, (float)ratio.latitude};
    };
}
