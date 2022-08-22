#include <plateau/io/mesh_merger.h>
#include <plateau/io/primary_city_object_types.h>
#include "polar_to_plane_cartesian.h"
#include "citygml/tesselator.h"
#include "citygml/texture.h"
#include "obj_writer.h"

/**
 * グリッド番号と、そのグリッドに属する CityObject のリストを対応付ける辞書です。
 */
using GridIdToObjsMap = std::map<int, std::list<std::unique_ptr<CityObjectWithImportID>>>;
using PolygonList = std::list<std::shared_ptr<const citygml::Polygon>>;

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
    std::unique_ptr<GridIdToObjsMap> initGridIdToObjsMap(const int gridNumX, const int gridNumY){
        int gridNum = gridNumX * gridNumY;
        auto gridIdToObjsMap = std::make_unique<GridIdToObjsMap>();
        for(int i=0; i<gridNum; i++){
            gridIdToObjsMap->emplace(i, std::list<std::unique_ptr<CityObjectWithImportID>>());
        }
        return std::move(gridIdToObjsMap);
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

    void FindAllPolygons(const Geometry& geom, std::unique_ptr<PolygonList>& polygons){
        unsigned int numChild = geom.getGeometriesCount();
        for(unsigned int i=0; i<numChild; i++){
            FindAllPolygons(geom.getGeometry(i), polygons);
        }

        // TODO 今は対象LODは2のみで決め打ちしているが、今後はLODを選択できるようにしたい
        if(geom.getLOD() != 2) return;

        unsigned int numPoly = geom.getPolygonsCount();
        for(unsigned int i=0; i<numPoly; i++){
            polygons->push_back(geom.getPolygon(i));
        }
    }

    /**
     * cityObj に含まれるポリゴンをすべて検索し、polygonsリストに追加します。
     * 子の CityObject は検索しません。
     * 子の Geometry は再帰的に検索します。
     */
    std::unique_ptr<PolygonList> FindAllPolygons(const CityObject& cityObj){
        auto polygons = std::make_unique<PolygonList>();
        unsigned int numGeom = cityObj.getGeometriesCount();
        for(unsigned int i=0; i<numGeom; i++){
            FindAllPolygons(cityObj.getGeometry(i), polygons);
        }
        return std::move(polygons);
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
     * また ImportID を割り振ります。
     */
    std::unique_ptr<GridIdToObjsMap> classifyCityObjsToGrid(const ConstCityObjects& cityObjs, const Envelope& cityEnvelope, int gridNumX, int gridNumY){
        auto gridIdToObjsMap = initGridIdToObjsMap(gridNumX, gridNumY);
        int primaryImportID = 0;
        for(auto co : cityObjs){
            int gridId = getGridId(cityEnvelope, cityObjPos(*co), gridNumX, gridNumY);
            auto cityObjWithImportID = std::make_unique<CityObjectWithImportID>(co, primaryImportID, -1);
            gridIdToObjsMap->at(gridId).push_back(std::move(cityObjWithImportID));
            primaryImportID++;
        }
        return std::move(gridIdToObjsMap);
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
     * cityObj の子を再帰的に検索して返します。
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

void MeshMerger::gridMerge(const CityModel &cityModel, const CityObject::CityObjectsType targetTypeMask, const int gridNumX, const int gridNumY, const std::shared_ptr<PlateauDllLogger> &logger) {

    // cityModel に含まれる 主要地物 をグリッドに分類します。
    auto& primaryCityObjs = cityModel.getAllCityObjectsOfType(targetTypeMask & PrimaryCityObjectTypes::getPrimaryTypeMask());
    auto& cityEnvelope = cityModel.getEnvelope();
    auto gridIdToObjsMap = classifyCityObjsToGrid( primaryCityObjs, cityEnvelope, gridNumX, gridNumY);

    // 主要地物の子（最小地物）を親と同じグリッドに追加します。
    // 意図はグリッドの端で同じ建物が分断されないようにするためです。
    for(auto& [gridId, primaryObjs] : *gridIdToObjsMap){
        for(auto& primaryObj : primaryObjs){
            int primaryID = primaryObj->getPrimaryImportID();
            auto minimumObjs = childCityObjects(*primaryObj->getCityObject());
            int secondaryID = 0;
            for(auto minimumObj : *minimumObjs){
                auto minimumObjWithID = std::make_unique<CityObjectWithImportID>(minimumObj, primaryID, secondaryID);
                gridIdToObjsMap->at(gridId).push_back(std::move(minimumObjWithID));
                secondaryID++;
            }
        }
    }

    // デバッグ用表示 : グリッド分類結果
//    for(const auto& pair : *gridIdToObjsMap){
//        std::cout << "[grid " << pair.first << "] ";
//        for(auto cityObj : pair.second){
//            std::cout << cityObj->getId() << ", ";
//        }
//        std::cout << std::endl;
//    }

    // グリッドごとにメッシュを結合します。
    auto gridPolygons = std::make_unique<GridMergeResult>();
    int gridNum = gridNumX * gridNumY;
    // グリッドごとのループ
    for(int i=0; i<gridNum; i++){
        // グリッド内でマージするポリゴンの新規作成
        auto gridPoly = std::make_unique<Mesh>("grid" + std::to_string(i), logger);
        auto& objsInGrid = gridIdToObjsMap->at(i);
        // グリッド内の各オブジェクトのループ
        for(auto& cityObj : objsInGrid){
            auto polygons = FindAllPolygons(*cityObj->getCityObject());
            // オブジェクト内の各ポリゴンのループ
            for(const auto& poly : *polygons){
                // 各ポリゴンを結合していきます。
                auto uv2 = TVec2f(cityObj->getPrimaryImportID() + 0.25, 0); // +0.25 する理由は、floatの誤差があっても四捨五入しても切り捨てても望みのint値を得られるように
                auto uv3 = TVec2f(cityObj->getSecondaryImportID() + 0.25, 0);
                gridPoly->Merge(*poly, uv2, uv3);
            }
        }

        gridPolygons->push_back(std::move(gridPoly));
    }

    // 街の範囲の中心を基準点とします。
    auto cityCenter = (cityEnvelope.getLowerBound() + cityEnvelope.getUpperBound()) / 2.0;
    polar_to_plane_cartesian().convert(cityCenter);
    cityCenter.z = 0.0; // ただし高さ方法は0を基準点とします。

    // 座標を変換します。
    for(auto& poly : *gridPolygons){
        auto numVert = poly->getVertices().size();
        for(int i=0; i<numVert; i++){
            auto pos = poly->getVertices().at(i);
            polar_to_plane_cartesian().convert(pos);
            // FIXME 変換部分だけ ObjWriterの機能を拝借しているけど、本質的には ObjWriter である必要はない。変換を別クラスに書き出した方が良い。
            // TODO AxesConversion, unit_scale は調整できるようにする
            pos = ObjWriter::convertPosition(pos, cityCenter, AxesConversion::WUN, 1.0);
            poly->getVertices().at(i) = pos;
        }

    }

    // デバッグ用表示 : マージしたポリゴンの情報
//    for(auto& gridPoly : *gridPolygons){
//        std::cout << "[" << gridPoly->getId() << "] ";
//        std::cout << "numVertices : " << gridPoly->getVertices().size() << std::endl;
//        std::cout << "multiTexture : ";
//        for(auto& idToTex : gridPoly->getMultiTexture()){
//            std::cout << "(" << idToTex.first << ", " << (idToTex.second->getUrl()) << "),   ";
//        }
//        std::cout << std::endl;
//    }

    lastGridMergeResult_ = std::move(gridPolygons);
}

MeshMerger::GridMergeResult & MeshMerger::getLastGridMergeResult() {
    return *lastGridMergeResult_;
}

CityObjectWithImportID::CityObjectWithImportID(const CityObject *const cityObject, int primaryImportID, int secondaryImportID) :
    cityObject(cityObject),
    primaryImportID(primaryImportID),
    secondaryImportID(secondaryImportID) {

}
