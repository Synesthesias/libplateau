#pragma once

#include "citygml/citymodel.h"
using namespace citygml;

namespace plateau::geometry{
    class GeometryUtils{
    public:
        /**
         * cityModel の中心点を返します。
         * ただし高さ方向の座標は 0 とし、東西南北の方向の座標のみ中心を指します。
         * そうでないと基準点が空中に浮かぶので気分的には良くないためです。
         * また GMLファイルから cityModel の Envelope を読み取れない場合は、
         * 中心点が分からないので原点座標を返します。
         */
        static TVec3d getCenterPoint(const CityModel& cityModel);
    };
}