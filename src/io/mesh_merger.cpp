#include <plateau/io/mesh_merger.h>
using IdToObjsMap = std::map<int, std::vector<const CityObject*>>;

namespace{
    static int getGridId(const Envelope& cityEnvelope, const TVec3d position, const int gridNumX, const int gridNumY){
        auto lower = cityEnvelope.getLowerBound();
        auto upper = cityEnvelope.getUpperBound();
        int gridX = (int)((position.x - lower.x) * gridNumX / (upper.x - lower.x));
        int gridY = (int)((position.y - lower.y) * gridNumY / (upper.y - lower.y));
        if(gridX < 0) gridX = 0;
        if(gridY < 0) gridY = 0;
        if(gridX >= gridNumX) gridX = gridNumX - 1;
        if(gridY >= gridNumY) gridY = gridNumY - 1;
        int gridId = gridX + gridY * gridNumX;
        return gridId;
    }

    static IdToObjsMap initIdToObjsMap(const int gridNumX, const int gridNumY){
        int gridNum = gridNumX * gridNumY;
        IdToObjsMap idToObjsMap;
        for(int i=0; i<gridNum; i++){
            idToObjsMap[i] = *new std::vector<const CityObject*>();
        }
        return idToObjsMap;
    }

    static const Polygon* FindFirstPolygon(const Geometry& geometry){
        int numPoly = geometry.getPolygonsCount();
        for(int i=0; i<numPoly; i++){
            auto poly = geometry.getPolygon(i);
            if(poly->getVertices().size() > 0) return poly.get();
        }
        int numGeom = geometry.getGeometriesCount();
        for(int i=0; i<numGeom; i++){
            auto found = FindFirstPolygon(geometry.getGeometry(i));
            if(found) return found;
        }
        return nullptr;
    }

    static const Polygon* FindFirstPolygon(const CityObject* cityObj){
        int numObj = cityObj->getChildCityObjectsCount();
        for(int i=0; i<numObj; i++){
            auto found = FindFirstPolygon(&cityObj->getChildCityObject(i));
            if(found) return found;
        }
        int numGeom = cityObj->getGeometriesCount();
        for(int i=0; i<numGeom; i++){
            auto found = FindFirstPolygon(cityObj->getGeometry(i));
            if(found) return found;
        }
        return nullptr;
    }

    static void classifyCityObjsToGrid(IdToObjsMap& idToObjsMap, const ConstCityObjects& cityObjs, const Envelope& cityEnvelope, int gridNumX, int gridNumY){
        for(auto co : cityObjs){
            auto& envelope = co->getEnvelope();
            TVec3d centerPos;
            if(envelope.validBounds()){
                centerPos = (envelope.getUpperBound() + envelope.getUpperBound())*0.5;
            }else{
                auto poly = FindFirstPolygon(co);
                if(poly){
                    centerPos = poly->getVertices().at(0);
                }else{
                    centerPos = TVec3d(-99,-99,-99);
                }
            }

            int gridId = getGridId(cityEnvelope, centerPos, gridNumX, gridNumY);
            idToObjsMap[gridId].push_back(co);
        }
    }
}

/*std::vector<PolygonWithUV2>*/void MeshMerger::GridMerge(const CityModel &cityModel, CityObject::CityObjectsType targetTypeMask, int gridNumX, int gridNumY) {
    auto& cityObjs = cityModel.getAllCityObjectsOfType(targetTypeMask);
    auto& cityEnvelope = cityModel.getEnvelope();
    auto idToObjsMap = initIdToObjsMap(gridNumX, gridNumY);
    classifyCityObjsToGrid(idToObjsMap, cityObjs, cityEnvelope, gridNumX, gridNumY);

    // Debug Print
    for(auto pair : idToObjsMap){
        std::cout << "[grid " << pair.first << "] ";
        for(auto cityObj : pair.second){
            std::cout << cityObj->getId() << ", ";
        }
        std::cout << std::endl;
    }

//    return *new std::vector<PolygonWithUV2>{};
}
