#include <plateau/polygon_mesh/polygon_mesh_utils.h>
#include "../io/polar_to_plane_cartesian.h"
#include "plateau/polygon_mesh/mesh.h"
#include "plateau/geometry/geo_reference.h"

namespace plateau::polygonMesh {
    using namespace citygml;

    namespace {
        void childCityObjectsRecursive(const CityObject& city_obj, std::list<const CityObject*>& child_objs) {
            child_objs.push_back(&city_obj);
            unsigned int num_child = city_obj.getChildCityObjectsCount();
            for (unsigned int i = 0; i < num_child; i++) {
                auto& child = city_obj.getChildCityObject(i);
                childCityObjectsRecursive(child, child_objs);
            }
        }
    }

    TVec3d GeometryUtils::getCenterPoint(const CityModel& city_model) {
        auto& envelope = city_model.getEnvelope();
        if (!envelope.validBounds()) {
            return TVec3d{0, 0, 0};
        }
        auto city_center = (envelope.getLowerBound() + envelope.getUpperBound()) / 2.0;
        return GeoReference().project(city_center);;
    }

/**
 * city_obj の子を再帰的に検索して返します。
 * ただし引数のcityObj自身は含めません。
 */
    std::list<const CityObject*> GeometryUtils::getChildCityObjectsRecursive(const CityObject& city_obj) {
        auto children = std::list<const CityObject*>();
        unsigned int num_child = city_obj.getChildCityObjectsCount();
        for (unsigned int i = 0; i < num_child; i++) {
            auto& child = city_obj.getChildCityObject(i);
            childCityObjectsRecursive(child, children);
        }
        return children;
    }
}
