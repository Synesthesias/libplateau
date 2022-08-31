#include "grid_merger.h"
#include <plateau/geometry/primary_city_object_types.h>
#include "../io/polar_to_plane_cartesian.h"
#include "plateau/io/obj_writer.h"
#include "../src/geometry/geometry_utils.h"

using namespace plateau::geometry;

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
    GridIDToObjsMap initGridIdToObjsMap(const int gridNumX, const int gridNumY){
        int gridNum = gridNumX * gridNumY;
        auto gridIdToObjsMap = GridIDToObjsMap();
        for(int i=0; i<gridNum; i++){
            gridIdToObjsMap.emplace(i, std::list<CityObjectWithImportID>());
        }
        return gridIdToObjsMap;
    }

    const citygml::Polygon* FindFirstPolygon(const Geometry& geometry, unsigned LOD){
        if(geometry.getLOD() != LOD) return nullptr;
        // 子の Polygon について再帰
        unsigned int numPoly = geometry.getPolygonsCount();
        for(unsigned int i=0; i<numPoly; i++){
            auto poly = geometry.getPolygon(i);
            if(!poly->getVertices().empty()) return poly.get();
        }
        // 子の Geometry について再帰
        unsigned int numGeom = geometry.getGeometriesCount();
        for(unsigned int i=0; i<numGeom; i++){
            auto found = FindFirstPolygon(geometry.getGeometry(i), LOD);
            if(found) return found;
        }
        return nullptr;
    }

    /**
     * cityObjのポリゴンであり、頂点数が1以上であるものを検索します。
     * 最初に見つかったポリゴンを返します。なければ nullptr を返します。
     */
    const citygml::Polygon* FindFirstPolygon(const CityObject* cityObj, unsigned LOD){
        // 子の CityObject について再帰
        unsigned int numObj = cityObj->getChildCityObjectsCount();
        for(unsigned int i=0; i<numObj; i++){
            auto found = FindFirstPolygon(&cityObj->getChildCityObject(i), LOD);
            if(found) return found;
        }
        // 子の Geometry について再帰
        unsigned int numGeom = cityObj->getGeometriesCount();
        for(unsigned int i=0; i<numGeom; i++){
            auto found = FindFirstPolygon(cityObj->getGeometry(i), LOD);
            if(found) return found;
        }
        return nullptr;
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
            auto poly = FindFirstPolygon(&cityObj, 0);
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
    GridIDToObjsMap classifyCityObjsToGrid(const ConstCityObjects& cityObjs, const Envelope& cityEnvelope, int gridNumX, int gridNumY){
        auto gridIdToObjsMap = initGridIdToObjsMap(gridNumX, gridNumY);
        int primaryImportID = 0;
        for(auto co : cityObjs){
            int gridId = getGridId(cityEnvelope, cityObjPos(*co), gridNumX, gridNumY);
            auto cityObjWithImportID = CityObjectWithImportID(co, primaryImportID, -1);
            gridIdToObjsMap.at(gridId).push_back(cityObjWithImportID);
            primaryImportID++;
        }
        return gridIdToObjsMap;
    }

}

GridMergeResult GridMerger::gridMerge(const CityModel &cityModel, const MeshExtractOptions &options, unsigned LOD) {
    // cityModel に含まれる 主要地物 をグリッドに分類します。
    const auto& primaryCityObjs = cityModel.getAllCityObjectsOfType(PrimaryCityObjectTypes::getPrimaryTypeMask());
    const auto& cityEnvelope = cityModel.getEnvelope();
    auto gridIdToObjsMap = classifyCityObjsToGrid( primaryCityObjs, cityEnvelope, options.gridCountOfSide, options.gridCountOfSide);

    // 主要地物の子（最小地物）を親と同じグリッドに追加します。
    // 意図はグリッドの端で同じ建物が分断されないようにするためです。
    for(const auto& pair : gridIdToObjsMap){
        unsigned gridId = pair.first;
        const auto& primaryObjs = pair.second;
        for(const auto& primaryObj : primaryObjs){
            int primaryID = primaryObj.getPrimaryImportID();
            auto atomicObjs = GeometryUtils::getChildCityObjectsRecursive(*primaryObj.getCityObject());
            int secondaryID = 0;
            for(auto atomicObj : atomicObjs){
                auto atomicObjWithID = CityObjectWithImportID{atomicObj, primaryID, secondaryID};
                gridIdToObjsMap.at(gridId).push_back(atomicObjWithID);
                secondaryID++;
            }
        }
    }

    // グリッドをさらに分割してグループにします。
    // グループの分割基準:
    // 今のLODをn、仕様上存在しうる最大LODをm として、各オブジェクトを次のグループに分けます。
    // - 同じオブジェクトが (0, 1, 2, ..., m) のLODであるポリゴンをどれも有する
    // - 同じオブジェクトが (0, 1, 2, ..., m-1) のLODであるポリゴンをどれも有し、mを有しない
    // - ...
    // - 同じオブジェクトが (0, 1) のLODであるポリゴンをどれも有し、(2, 3, ..., m)のLODであるポリゴンをどれも有しない
    // - 同じオブジェクトが (0) のLODであるポリゴンを有し、(1, 2, 3,  ..., m) のLODであるポリゴンをどれも有しない
    // 今の仕様上、mは3なので、各グリッドは最大4つに分割されます。
    // したがって、grid 0 と group 0 to 3 が対応します。 grid1 と group 4 to 7 が対応します。
    // 一般化すると、 grid i と group (i*m) to (i*(m+1)-1)が対応します。
    // 仕様上、あるオブジェクトのLOD i が存在すれば、同じオブジェクトの LOD 0 to i-1 がすべて存在します。したがって、各オブジェクトは必ず上記グループのどれか1つに該当するはずです。
    // そのようにグループ分けする利点は、
    // 「高いLODを表示したいけど、低いLODにしか対応していない箇所が穴になってしまう」という状況で、穴をちょうど埋める範囲の低LODグループが存在することです。
    auto groupIDToObjsMap = GroupIDToObjsMap();
    for(const auto& gridObjsPair : gridIdToObjsMap){
        auto gridID = gridObjsPair.first;
        const auto& gridObjs = gridObjsPair.second;
        for(const auto& obj : gridObjs){
            // この CityObj について、最大でどのLODまで存在するか確認します。
            unsigned maxLODInObj = GeometryUtils::MaxLODInSpecification;
            for(unsigned checkLOD = LOD+1; checkLOD <= GeometryUtils::MaxLODInSpecification; ++checkLOD){
                bool polygonInLODExists = (checkLOD == LOD) || (FindFirstPolygon(obj.getCityObject(), checkLOD) != nullptr);
                if(!polygonInLODExists){
                    maxLODInObj = checkLOD -1;
                    break;
                }
            }
            // グループに追加します。
            unsigned groupID = gridID * (GeometryUtils::MaxLODInSpecification+1) + maxLODInObj;
            if(groupIDToObjsMap.find(groupID) == groupIDToObjsMap.end()){
                groupIDToObjsMap[groupID] = std::list<CityObjectWithImportID>();
            }
            groupIDToObjsMap.at(groupID).push_back(obj);
        }
    }

    // グループごとにメッシュを結合します。
    auto mergedMeshes = GridMergeResult();
    // グループごとのループ
    for(const auto& group : groupIDToObjsMap){
        auto groupID = group.first;
        const auto& groupObjs = group.second;
        // グループ内でマージする Mesh の新規作成
        auto groupMesh = Mesh("group" + std::to_string(groupID));
        // グループ内の各オブジェクトのループ
        for(const auto& cityObj : groupObjs){
            auto polygons = GeometryUtils::findAllPolygons(*cityObj.getCityObject(), LOD);
            // オブジェクト内の各ポリゴンのループ
            for(const auto& poly : polygons){
                // 各ポリゴンを結合していきます。
                // importID をメッシュに残すためにuv2, uv3 を利用しています。UVなので2次元floatの値を取りますが、実際に伝えたい値はUVごとに1つのintです。
                const auto uv2 = TVec2f((float)(cityObj.getPrimaryImportID()) + (float)0.25, 0); // +0.25 する理由は、floatの誤差があっても四捨五入しても切り捨てても望みのint値を得られるためです。
                const auto uv3 = TVec2f((float)(cityObj.getSecondaryImportID()) + (float)0.25, 0);
                groupMesh.merge(*poly, options, uv2, uv3);
            }
        }
        mergedMeshes.emplace(groupID, std::move(groupMesh));
    }
    return mergedMeshes;
}
