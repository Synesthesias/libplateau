#include <plateau/polygon_mesh/polygon_mesh_utils.h>
#include "plateau/polygon_mesh/mesh.h"
#include "plateau/geometry/geo_reference.h"
#include "citygml/citymodel.h"

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

        const citygml::Polygon* findFirstPolygonInGeometry(const Geometry& geometry, unsigned lod) {
            if (geometry.getLOD() != lod) return nullptr;
            // 子の Polygon について再帰
            unsigned int num_poly = geometry.getPolygonsCount();
            for (unsigned int i = 0; i < num_poly; i++) {
                auto poly = geometry.getPolygon(i);
                if (!poly->getVertices().empty()) return poly.get();
            }
            // 子の Geometry について再帰
            unsigned int num_geom = geometry.getGeometriesCount();
            for (unsigned int i = 0; i < num_geom; i++) {
                auto found = findFirstPolygonInGeometry(geometry.getGeometry(i), lod);
                if (found) return found;
            }
            return nullptr;
        }
    }


    TVec3d PolygonMeshUtils::cityObjPos(const CityObject& city_obj) {
        auto& envelope = city_obj.getEnvelope();
        if (envelope.validBounds()) {
            // city_obj の envelope 情報がGMLファイル中に記載されていれば、その中心を返します。
            // しかし、CityObjectごとに記載されているケースは少ないです。
            return (envelope.getLowerBound() + envelope.getUpperBound()) * 0.5;
        } else {
            // envelope がなければ、ポリゴンを検索して見つかった最初の頂点の位置を返します。
            for (int lod = 0; lod <= max_lod_in_specification_; lod++) {
                auto poly = findFirstPolygon(&city_obj, lod);
                if (poly) {
                    return poly->getVertices().at(0);
                }
            }
        }
        // 位置が不明
        throw std::invalid_argument("Could not find position of CityObject.");
    }

    TVec3d PolygonMeshUtils::getCenterPoint(const CityModel& city_model, int coordinate_zone_id) {
        auto& envelope = city_model.getEnvelope();
        if (!envelope.validBounds()) {
            return TVec3d{0, 0, 0};
        }
        auto city_center = (envelope.getLowerBound() + envelope.getUpperBound()) / 2.0;
        return GeoReference(coordinate_zone_id).project(city_center);
    }

    /**
     * city_obj の子を再帰的に検索して返します。
     * ただし引数のcityObj自身は含めません。
     */
    std::list<const CityObject*> PolygonMeshUtils::getChildCityObjectsRecursive(const CityObject& city_obj) {
        auto children = std::list<const CityObject*>();
        unsigned int num_child = city_obj.getChildCityObjectsCount();
        for (unsigned int i = 0; i < num_child; i++) {
            auto& child = city_obj.getChildCityObject(i);
            childCityObjectsRecursive(child, children);
        }
        return children;
    }

    const citygml::Polygon* PolygonMeshUtils::findFirstPolygon(const CityObject* city_obj, unsigned lod) {
        // 子の CityObject について再帰
        unsigned int num_obj = city_obj->getChildCityObjectsCount();
        for (unsigned int i = 0; i < num_obj; i++) {
            auto found = findFirstPolygon(&city_obj->getChildCityObject(i), lod);
            if (found) return found;
        }
        // 子の Geometry について再帰
        unsigned int num_geom = city_obj->getGeometriesCount();
        for (unsigned int i = 0; i < num_geom; i++) {
            auto found = findFirstPolygonInGeometry(city_obj->getGeometry(i), lod);
            if (found) return found;
        }
        return nullptr;
    }
}
