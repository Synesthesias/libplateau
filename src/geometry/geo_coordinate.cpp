#include <plateau/geometry/geo_coordinate.h>
#include <plateau/polygon_mesh/polygon_mesh_utils.h>
#include <cmath>

namespace plateau::geometry {
    using namespace citygml;
    using namespace polygonMesh;

    bool Extent::contains(GeoCoordinate point) const {
        return min.latitude <= point.latitude &&
               max.latitude >= point.latitude &&
               min.longitude <= point.longitude &&
               max.longitude >= point.longitude &&
               min.height <= point.height &&
               max.height >= point.height;
    }

    bool Extent::contains(TVec3d point) const {
        return contains(GeoCoordinate(point.x, point.y, point.z));
    }

    bool Extent::contains(const CityObject& city_obj) const{
        try{
            auto pos = PolygonMeshUtils::cityObjPos(city_obj);
            return contains(pos);
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
}
