#include "geometry_utils.h"
#include "../io/polar_to_plane_cartesian.h"
#include "plateau/geometry/mesh.h"

using namespace plateau::geometry;

namespace{
    void childCityObjectsRecursive(const CityObject& city_obj, std::list<const CityObject*>& child_objs){
        child_objs.push_back(&city_obj);
        unsigned int num_child = city_obj.getChildCityObjectsCount();
        for(unsigned int i=0; i < num_child; i++){
            auto& child = city_obj.getChildCityObject(i);
            childCityObjectsRecursive(child, child_objs);
        }
    }
}

TVec3d GeometryUtils::getCenterPoint(const CityModel &city_model) {
    auto& envelope = city_model.getEnvelope();
    if(!envelope.validBounds()){
        return TVec3d{0,0,0};
    }
    auto city_center = (envelope.getLowerBound() + envelope.getUpperBound()) / 2.0;
    polar_to_plane_cartesian().convert(city_center);
    return city_center;
}


std::list<const Polygon *> GeometryUtils::findAllPolygons(const CityObject &city_obj, unsigned lod) {
    auto polygons = std::list<const citygml::Polygon*>();
    unsigned int num_geom = city_obj.getGeometriesCount();
    for(unsigned int i=0; i < num_geom; i++){
        findAllPolygons(city_obj.getGeometry(i), polygons, lod);
    }
    return std::move(polygons);
}

void GeometryUtils::findAllPolygons(const Geometry &geom, std::list<const citygml::Polygon *> &polygons, unsigned lod) {
    unsigned int num_child = geom.getGeometriesCount();
    for(unsigned int i=0; i < num_child; i++){
        findAllPolygons(geom.getGeometry(i), polygons, lod);
    }

    if(geom.getLOD() != lod) return;

    unsigned int num_poly = geom.getPolygonsCount();
    for(unsigned int i=0; i < num_poly; i++){
        polygons.push_back(geom.getPolygon(i).get());
    }
}

/**
 * city_obj の子を再帰的に検索して返します。
 * ただし引数のcityObj自身は含めません。
 */
std::list<const CityObject*> GeometryUtils::getChildCityObjectsRecursive(const CityObject& city_obj ){
    auto children = std::list<const CityObject*>();
    unsigned int num_child = city_obj.getChildCityObjectsCount();
    for(unsigned int i=0; i < num_child; i++){
        auto& child = city_obj.getChildCityObject(i);
        childCityObjectsRecursive(child, children);
    }
    return children;
}
