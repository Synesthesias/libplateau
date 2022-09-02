#pragma once
#include <citygml/vecs.hpp>
#include "geo_coordinate.h"

namespace plateau::geometry{

    /**
     * 座標変換の基準を保持し、座標変換します。
     *
     * zoneID についてはこちらを参照してください :
     * https://www.gsi.go.jp/sokuchikijun/jpc.html
     */
    class GeoReference{
    public:
        GeoReference(TVec3d reference_point, float unit_scale, CoordinateSystem coordinate_system,
                     int zone_id  = default_zone_id_);
        TVec3d project(GeoCoordinate point);

        // TODO 未実装
        // GeoCoordinate unproject(TVec3d point);

        void setReferencePoint(TVec3d point);
        TVec3d getReferencePoint();
        int getZoneID();
        void setZoneID(int value);

    private:
        TVec3d reference_point_;
        int zone_id_;
        CoordinateSystem coordinate_system_;
        float unit_scale_;

        /**
         * 関東地方で歪みの少ない座標系です。
         */
        static int constexpr default_zone_id_ = 9;
    };
}
