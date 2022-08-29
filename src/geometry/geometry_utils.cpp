#include <plateau/geometry/geometry_utils.h>
#include "plateau/io/polar_to_plane_cartesian.h"
#include "plateau/geometry/mesh.h"

using namespace plateau::geometry;

TVec3d GeometryUtils::getCenterPoint(const CityModel &cityModel) {
    auto& envelope = cityModel.getEnvelope();
    if(!envelope.validBounds()){
        return TVec3d{0,0,0};
    }
    auto cityCenter = (envelope.getLowerBound() + envelope.getUpperBound()) / 2.0;
    polar_to_plane_cartesian().convert(cityCenter);
    cityCenter.z = 0.0;
    return cityCenter;
}


std::list<const Polygon *> GeometryUtils::findAllPolygons(const CityObject &cityObj, unsigned LOD) {
    auto polygons = std::list<const citygml::Polygon*>();
    unsigned int numGeom = cityObj.getGeometriesCount();
    for(unsigned int i=0; i<numGeom; i++){
        findAllPolygons(cityObj.getGeometry(i), polygons, LOD);
    }
    return std::move(polygons);
}

void GeometryUtils::findAllPolygons(const Geometry &geom, std::list<const citygml::Polygon *> &polygons, unsigned LOD) {
    unsigned int numChild = geom.getGeometriesCount();
    for(unsigned int i=0; i<numChild; i++){
        findAllPolygons(geom.getGeometry(i), polygons, LOD);
    }

    if(geom.getLOD() != LOD) return;

    unsigned int numPoly = geom.getPolygonsCount();
    for(unsigned int i=0; i<numPoly; i++){
        polygons.push_back(geom.getPolygon(i).get());
    }
}

/**
 * cityObj の子を再帰的に検索して返します。
 * ただし引数のcityObj自身は含めません。
 */
std::list<const CityObject*> GeometryUtils::getChildCityObjectsRecursive(const CityObject& cityObj ){
    auto children = std::list<const CityObject*>();
    unsigned int numChild = cityObj.getChildCityObjectsCount();
    for(unsigned int i=0; i<numChild; i++){
        auto& child = cityObj.getChildCityObject(i);
        GeometryUtils::childCityObjectsRecursive(child, children);
    }
    return children;
}

void GeometryUtils::childCityObjectsRecursive(const CityObject& cityObj, std::list<const CityObject*>& childObjs){
    childObjs.push_back(&cityObj);
    unsigned int numChild = cityObj.getChildCityObjectsCount();
    for(unsigned int i=0; i<numChild; i++){
        auto& child = cityObj.getChildCityObject(i);
        childCityObjectsRecursive(child, childObjs);
    }
}