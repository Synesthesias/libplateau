#pragma once

#include <list>
#include "citygml/citymodel.h"
#include "citygml/polygon.h"
#include "mesh.h"

namespace plateau::geometry{
    class GeometryUtils{
    public:
        /// 仕様上存在しうる最大LODです。 LODは0から始まるので、LODのパターン数は (この数 +1)です。
        static constexpr int MaxLODInSpecification = 3;

        /**
         * cityModel の中心点を返します。
         * ただし高さ方向の座標は 0 とし、東西南北の方向の座標のみ中心を指します。
         * そうでないと基準点が空中に浮かぶので気分的には良くないためです。
         * また GMLファイルから cityModel の Envelope を読み取れない場合は、
         * 中心点が分からないので原点座標を返します。
         */
        static TVec3d getCenterPoint(const CityModel& cityModel);

        /**
         * cityObj に含まれるポリゴンをすべて検索し、リストで返します。
         * 子の CityObject は検索しません。
         * 子の Geometry は再帰的に検索します。
         */
        static std::list<const citygml::Polygon *> findAllPolygons(const CityObject &cityObj, unsigned LOD);

        /**
         * findAllPolygons のジオメトリを対象とする版です。
         * 結果は引数の polygons に格納します。
         */
        static void findAllPolygons(const Geometry &geom, std::list<const citygml::Polygon *> &polygons, unsigned LOD);

        /**
         * cityObj の子を再帰的に検索して返します。
         * ただし引数のcityObj自身は含めません。
         */
        static std::list<const CityObject*> getChildCityObjectsRecursive(const CityObject& cityObj);

    private:

        static void childCityObjectsRecursive(const CityObject& cityObj, std::list<const CityObject*>& childObjs);
    };
}