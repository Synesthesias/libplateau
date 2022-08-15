#include <plateau/io/mesh_merger.h>
#include "citygml/tesselator.h"

using GridIdToObjsMap = std::map<int, std::vector<const CityObject*>>;
using PolygonVector = std::vector<std::shared_ptr<const citygml::Polygon>>;

namespace{
    /**
     * cityModelのEnvelope(範囲)を指定のグリッド数(x,y)で分割したとき、
     * positionは何番目のグリッドに属するかを計算します。
     */
    int getGridId(const Envelope& cityEnvelope, const TVec3d position, const int gridNumX, const int gridNumY){
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

    /**
     * key が グリッド番号であり、 value が　CityObjectのvector であるmapを初期化します。
     */
    GridIdToObjsMap initGridIdToObjsMap(const int gridNumX, const int gridNumY){
        int gridNum = gridNumX * gridNumY;
        GridIdToObjsMap gridIdToObjsMap;
        for(int i=0; i<gridNum; i++){
            gridIdToObjsMap[i] = *new std::vector<const CityObject*>();
        }
        return gridIdToObjsMap;
    }

    const citygml::Polygon* FindFirstPolygon(const Geometry& geometry){
        auto numPoly = geometry.getPolygonsCount();
        for(int i=0; i<numPoly; i++){
            auto poly = geometry.getPolygon(i);
            if(!poly->getVertices().empty()) return poly.get();
        }
        auto numGeom = geometry.getGeometriesCount();
        for(int i=0; i<numGeom; i++){
            auto found = FindFirstPolygon(geometry.getGeometry(i));
            if(found) return found;
        }
        return nullptr;
    }

    /**
     * cityObjのポリゴンであり、頂点数が1以上であるものを検索します。
     * 最初に見つかったポリゴンを返します。なければ nullptr を返します。
     */
    const citygml::Polygon* FindFirstPolygon(const CityObject* cityObj){
        auto numObj = cityObj->getChildCityObjectsCount();
        for(int i=0; i<numObj; i++){
            auto found = FindFirstPolygon(&cityObj->getChildCityObject(i));
            if(found) return found;
        }
        auto numGeom = cityObj->getGeometriesCount();
        for(int i=0; i<numGeom; i++){
            auto found = FindFirstPolygon(cityObj->getGeometry(i));
            if(found) return found;
        }
        return nullptr;
    }

    void FindAllPolygons(const Geometry& geom, PolygonVector& polygons){
        auto numChild = geom.getGeometriesCount();
        for(int i=0; i<numChild; i++){
            FindAllPolygons(geom.getGeometry(i), polygons);
        }
        auto numPoly = geom.getPolygonsCount();
        for(int i=0; i<numPoly; i++){
            polygons.push_back(geom.getPolygon(i));
        }
    }

    void FindAllPolygons(const CityObject& cityObj, PolygonVector& polygons){
        auto numChild = cityObj.getChildCityObjectsCount();
        for(int i=0; i<numChild; i++){
            FindAllPolygons(cityObj.getChildCityObject(i), polygons);
        }
        auto numGeom = cityObj.getGeometriesCount();
        for(int i=0; i<numGeom; i++){
            FindAllPolygons(cityObj.getGeometry(i), polygons);
        }
    }



    /**
     * cityObjの位置を表現するにふさわしい1点の座標を返します。
     */
    TVec3d cityObjPos(const CityObject& cityObj){
        auto& envelope = cityObj.getEnvelope();
        if(envelope.validBounds()){
            // cityObj の envelope がGMLファイル中に記載されていれば、その中心を返します。
            // しかし、CityObjectごとに記載されているケースは少ないです。
            return (envelope.getLowerBound() + envelope.getUpperBound())*0.5;
        }else{
            // envelope がなければ、ポリゴンを検索して見つかった最初の頂点の位置を返します。
            auto poly = FindFirstPolygon(&cityObj);
            if(poly){
                return poly->getVertices().at(0);
            }
        }
        return TVec3d{-999,-999,-999};
    }

    /**
     * cityObjs の各CityObjectが位置の上でどのグリッドに属するかを求め、gridIdToObjsMapに追加することでグリッド分けします。
     */
    void classifyCityObjsToGrid(GridIdToObjsMap& gridIdToObjsMap, const ConstCityObjects& cityObjs, const Envelope& cityEnvelope, int gridNumX, int gridNumY){
        for(auto co : cityObjs){
            int gridId = getGridId(cityEnvelope, cityObjPos(*co), gridNumX, gridNumY);
            gridIdToObjsMap[gridId].push_back(co);
        }
    }


}

void MeshMerger::gridMerge(const CityModel *cityModel, CityObject::CityObjectsType targetTypeMask, int gridNumX, int gridNumY, std::shared_ptr<PlateauDllLogger> logger) {

    auto& cityObjs = cityModel->getAllCityObjectsOfType(targetTypeMask);
    auto& cityEnvelope = cityModel->getEnvelope();
    auto gridIdToObjsMap = initGridIdToObjsMap(gridNumX, gridNumY);
    classifyCityObjsToGrid(gridIdToObjsMap, cityObjs, cityEnvelope, gridNumX, gridNumY);

    // Debug Print
    for(auto pair : gridIdToObjsMap){
        std::cout << "[grid " << pair.first << "] ";
        for(auto cityObj : pair.second){
            std::cout << cityObj->getId() << ", ";
        }
        std::cout << std::endl;
    }

    auto gridPolygons = std::make_shared<std::vector<PolygonWithUV2*>>();
    int gridNum = gridNumX * gridNumY;
    for(int i=0; i<gridNum; i++){
        auto gridPoly = new PolygonWithUV2("grid" + std::to_string(i), logger);
        int numObjInGrid = gridIdToObjsMap[i].size();
        for(int j=0; j<numObjInGrid; j++){
            auto cityObj = gridIdToObjsMap[i].at(j);
            auto polygons = PolygonVector();
            FindAllPolygons(*cityObj, polygons);
            auto numPoly = polygons.size();
            for(int k=0; k<numPoly; k++){
                auto poly = polygons.at(k).get();
                gridPoly->Merge(*poly);
            }
        }
        gridPolygons->push_back(gridPoly);
    }
    
    // Debug Print
    for(auto gridPoly : *gridPolygons){
        std::cout << "[" << gridPoly->getId() << "] ";
        std::cout << "numVertices : " << gridPoly->getVertices().size() << std::endl;
    }
    lastGridMergeResult_ = gridPolygons;
}

MeshMerger::GridMergeResult MeshMerger::getLastGridMergeResult() {
    return lastGridMergeResult_;
}
