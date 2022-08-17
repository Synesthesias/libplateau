#include <plateau/io/mesh_merger.h>
#include <plateau/io/primary_city_object_types.h>
#include "polar_to_plane_cartesian.h"
#include "citygml/tesselator.h"
#include "citygml/texture.h"
#include "obj_writer.h"

using GridIdToObjsMap = std::map<int, std::list<const CityObject*>>;
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
     * key が グリッド番号であり、 value は　CityObjectのvector であるmapを初期化します。
     */
    GridIdToObjsMap* initGridIdToObjsMap(const int gridNumX, const int gridNumY){
        int gridNum = gridNumX * gridNumY;
        auto gridIdToObjsMap = new GridIdToObjsMap;
        for(int i=0; i<gridNum; i++){
            gridIdToObjsMap->emplace(i, std::list<const CityObject*>());
        }
        return gridIdToObjsMap;
    }

    const citygml::Polygon* FindFirstPolygon(const Geometry& geometry){
        unsigned int numPoly = geometry.getPolygonsCount();
        for(unsigned int i=0; i<numPoly; i++){
            auto poly = geometry.getPolygon(i);
            if(!poly->getVertices().empty()) return poly.get();
        }
        unsigned int numGeom = geometry.getGeometriesCount();
        for(unsigned int i=0; i<numGeom; i++){
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
        unsigned int numObj = cityObj->getChildCityObjectsCount();
        for(unsigned int i=0; i<numObj; i++){
            auto found = FindFirstPolygon(&cityObj->getChildCityObject(i));
            if(found) return found;
        }
        unsigned int numGeom = cityObj->getGeometriesCount();
        for(unsigned int i=0; i<numGeom; i++){
            auto found = FindFirstPolygon(cityObj->getGeometry(i));
            if(found) return found;
        }
        return nullptr;
    }

    void FindAllPolygons(const Geometry& geom, PolygonVector& polygons){
        unsigned int numChild = geom.getGeometriesCount();
        for(unsigned int i=0; i<numChild; i++){
            FindAllPolygons(geom.getGeometry(i), polygons);
        }
        unsigned int numPoly = geom.getPolygonsCount();
        for(unsigned int i=0; i<numPoly; i++){
            polygons.push_back(geom.getPolygon(i));
        }
    }

    /**
     * cityObj に含まれるポリゴンをすべて検索し、polygonsリストに追加します。
     * 子の CityObject は検索しません。
     * 子の Geometry は再帰的に検索します。
     */
    void FindAllPolygons(const CityObject* cityObj, PolygonVector& polygons){
        unsigned int numGeom = cityObj->getGeometriesCount();
        for(unsigned int i=0; i<numGeom; i++){
            FindAllPolygons(cityObj->getGeometry(i), polygons);
        }
    }



    /**
     * cityObjの位置を表現するにふさわしい1点の座標を返します。
     */
    TVec3d cityObjPos(const CityObject& cityObj){
        auto& envelope = cityObj.getEnvelope();
        if(envelope.validBounds()){
            // cityObj の envelope 情報がGMLファイル中に記載されていれば、その中心を返します。
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
    void classifyCityObjsToGrid(GridIdToObjsMap* gridIdToObjsMap, const ConstCityObjects& cityObjs, const Envelope& cityEnvelope, int gridNumX, int gridNumY){
        for(auto co : cityObjs){
            int gridId = getGridId(cityEnvelope, cityObjPos(*co), gridNumX, gridNumY);
            gridIdToObjsMap->at(gridId).push_back(co);
        }
    }

    void childCityObjectsRecursive(const CityObject& cityObj, std::list<const CityObject*>& childObjs){
        childObjs.push_back(&cityObj);
        unsigned int numChild = cityObj.getChildCityObjectsCount();
        for(unsigned int i=0; i<numChild; i++){
            auto& child = cityObj.getChildCityObject(i);
            childCityObjectsRecursive(child, childObjs);
        }
    }

    /**
     * cityObj の子を再帰的に検索して 引数のvector に格納します。
     * ただし引数のcityObj自身は含めません。
     */
    std::unique_ptr<std::list<const CityObject*>> childCityObjects(const CityObject& cityObj ){
        auto children = std::make_unique<std::list<const CityObject*>>();
        unsigned int numChild = cityObj.getChildCityObjectsCount();
        for(unsigned int i=0; i<numChild; i++){
            auto& child = cityObj.getChildCityObject(i);
            childCityObjectsRecursive(child, *children);
        }
        return std::move(children);
    }


}

void MeshMerger::gridMerge(const CityModel *cityModel, CityObject::CityObjectsType targetTypeMask, int gridNumX, int gridNumY, std::shared_ptr<PlateauDllLogger> logger) {

    // cityModel に含まれる 主要地物 をグリッドに分類します。
    auto& primaryCityObjs = cityModel->getAllCityObjectsOfType(targetTypeMask & PrimaryCityObjectTypes::getPrimaryTypeMask());
    auto& cityEnvelope = cityModel->getEnvelope();
    auto gridIdToObjsMap = initGridIdToObjsMap(gridNumX, gridNumY);
    classifyCityObjsToGrid(gridIdToObjsMap, primaryCityObjs, cityEnvelope, gridNumX, gridNumY);

    // 主要地物の子（最小地物）を親と同じグリッドに追加します。
    // 意図はグリッドの端で同じ建物が分断されないようにするためです。
    for(auto& [gridId, primaryObjs] : *gridIdToObjsMap){
        for(auto primaryObj : primaryObjs){
            auto minimumObjs = childCityObjects(*primaryObj);
            for(auto minimumObj : *minimumObjs){
                gridIdToObjsMap->at(gridId).push_back(minimumObj);
            }
        }
    }

    // デバッグ用表示 : グリッド分類結果
    for(const auto& pair : *gridIdToObjsMap){
        std::cout << "[grid " << pair.first << "] ";
        for(auto cityObj : pair.second){
            std::cout << cityObj->getId() << ", ";
        }
        std::cout << std::endl;
    }

    // グリッドごとにメッシュを結合します。
    auto gridPolygons = std::make_shared<std::vector<PlateauPolygon*>>();
    int gridNum = gridNumX * gridNumY;
    // グリッドごとのループ
    for(int i=0; i<gridNum; i++){
        // グリッド内でマージするポリゴンの新規作成
        auto gridPoly = new PlateauPolygon("grid" + std::to_string(i), logger);
        // グリッド内の各オブジェクトのループ
        for(auto cityObj : gridIdToObjsMap->at(i)){
            auto polygons = PolygonVector();
            FindAllPolygons(cityObj, polygons);
            auto numPoly = polygons.size();
            // オブジェクト内の各ポリゴンのループ
            for(int k=0; k<numPoly; k++){
                auto poly = polygons.at(k).get();
                // 各ポリゴンを結合していきます。
                gridPoly->Merge(*poly);
            }
        }

        gridPolygons->push_back(gridPoly);
    }

    // 座標を変換します。
    for(auto poly : *gridPolygons){
        auto numVert = poly->getVertices().size();
        for(int i=0; i<numVert; i++){
            auto& pos = poly->getVertices().at(i);
            polar_to_plane_cartesian().convert(pos);
            pos = ObjWriter::convertPosition(pos, TVec3d(0, 0, 0), AxesConversion::WUN, 1.0);
            poly->getVertices().at(i) = pos;
        }

    }

    // デバッグ用表示 : マージしたポリゴンの情報
    for(auto gridPoly : *gridPolygons){
        std::cout << "[" << gridPoly->getId() << "] ";
        std::cout << "numVertices : " << gridPoly->getVertices().size() << std::endl;
        std::cout << "multiTexture : ";
        for(auto& idToTex : gridPoly->getMultiTexture()){
            std::cout << "(" << idToTex.first << ", " << (idToTex.second->getUrl()) << "),   ";
        }
        std::cout << std::endl;
    }
    lastGridMergeResult_ = gridPolygons;
    // TODO deleteするよりスマポに直す
    delete gridIdToObjsMap;
}

MeshMerger::GridMergeResult MeshMerger::getLastGridMergeResult() {
    return lastGridMergeResult_;
}
