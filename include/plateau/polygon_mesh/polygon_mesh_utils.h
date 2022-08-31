#pragma once

#include <list>
#include "citygml/citymodel.h"
#include "citygml/polygon.h"
#include <plateau/polygon_mesh/mesh.h>

namespace plateau::polygonMesh {

    class GeometryUtils {
    public:
        /// 仕様上存在しうる最大LODです。 LODは0から始まるので、LODのパターン数は (この数 +1)です。
        static constexpr int max_lod_in_specification_ = 3;

        /**
         * city_model の中心点を返します。
         * また GMLファイルから city_model の Envelope を読み取れない場合は、
         * 中心点が分からないので原点座標を返します。
         */
        static TVec3d getCenterPoint(const citygml::CityModel& city_model);

        /**
         * city_obj の子を再帰的に検索して返します。
         * ただし引数のcityObj自身は含めません。
         */
        static std::list<const citygml::CityObject*> getChildCityObjectsRecursive(const citygml::CityObject& city_obj);
    };
}
