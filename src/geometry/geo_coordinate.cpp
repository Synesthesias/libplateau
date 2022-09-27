#include <plateau/geometry/geo_coordinate.h>
#include <plateau/polygon_mesh/polygon_mesh_utils.h>

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

    GeoCoordinate Extent::centerPoint() const {
        auto ret = GeoCoordinate(
                (min.latitude + max.latitude) * 0.5,
                (min.longitude + max.longitude) * 0.5,
                (min.height + max.height) * 0.5
                );
        return ret;
    }
}
