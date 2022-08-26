#include <plateau/geometry/grid_merger.h>
#include "plateau/io/primary_city_object_types.h"
#include "plateau/io/polar_to_plane_cartesian.h"
#include "plateau/io/obj_writer.h"
#include "plateau/geometry/geometry_utils.h"

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
    GridIdToObjsMap initGridIdToObjsMap(const int gridNumX, const int gridNumY){
        int gridNum = gridNumX * gridNumY;
        auto gridIdToObjsMap = GridIdToObjsMap();
        for(int i=0; i<gridNum; i++){
            gridIdToObjsMap.emplace(i, std::list<CityObjectWithImportID>());
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
    GridIdToObjsMap classifyCityObjsToGrid(const ConstCityObjects& cityObjs, const Envelope& cityEnvelope, int gridNumX, int gridNumY){
        auto gridIdToObjsMap = initGridIdToObjsMap(gridNumX, gridNumY);
        int primaryImportID = 0;
        for(auto co : cityObjs){
            int gridId = getGridId(cityEnvelope, cityObjPos(*co), gridNumX, gridNumY);
            auto cityObjWithImportID = CityObjectWithImportID(co, primaryImportID, -1);
            gridIdToObjsMap.at(gridId).push_back(cityObjWithImportID);
            primaryImportID++;
        }
        return std::move(gridIdToObjsMap);
    }

}

GridMergeResult GridMerger::gridMerge(const CityModel &cityModel, const MeshExtractOptions &options) {

    // cityModel に含まれる 主要地物 をグリッドに分類します。
    auto& primaryCityObjs = cityModel.getAllCityObjectsOfType(PrimaryCityObjectTypes::getPrimaryTypeMask());
    auto& cityEnvelope = cityModel.getEnvelope();
    auto gridIdToObjsMap = classifyCityObjsToGrid( primaryCityObjs, cityEnvelope, options.gridCountOfSide, options.gridCountOfSide);

    // 主要地物の子（最小地物）を親と同じグリッドに追加します。
    // 意図はグリッドの端で同じ建物が分断されないようにするためです。
    for(const auto& [gridId, primaryObjs] : gridIdToObjsMap){
        for(auto& primaryObj : primaryObjs){
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


    // グリッドごとにメッシュを結合します。
    auto gridMeshes = GridMergeResult();
    int gridNum = options.gridCountOfSide * options.gridCountOfSide;
    // グリッドごとのループ
    for(int i=0; i<gridNum; i++){
        // グリッド内でマージするポリゴンの新規作成
        auto gridMesh = Mesh("grid" + std::to_string(i));
        auto& objsInGrid = gridIdToObjsMap.at(i);
        // グリッド内の各オブジェクトのループ
        for(auto& cityObj : objsInGrid){
            auto polygons = GeometryUtils::findAllPolygons(*cityObj.getCityObject(), options.minLOD, options.maxLOD);
            // オブジェクト内の各ポリゴンのループ
            for(const auto& poly : polygons){
                // 各ポリゴンを結合していきます。
                // importID をメッシュに残すためにuv2, uv3 を利用しています。UVなので2次元floatの値を取りますが、実際に伝えたい値はUVごとに1つのintです。
                const auto uv2 = TVec2f((float)(cityObj.getPrimaryImportID()) + (float)0.25, 0); // +0.25 する理由は、floatの誤差があっても四捨五入しても切り捨てても望みのint値を得られるように
                const auto uv3 = TVec2f((float)(cityObj.getSecondaryImportID()) + (float)0.25, 0);
                gridMesh.merge(*poly, options, uv2, uv3);
            }
        }

        gridMeshes.push_back(std::move(gridMesh));
    }


    return gridMeshes;
}