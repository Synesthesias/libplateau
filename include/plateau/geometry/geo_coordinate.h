#pragma once

#include "citygml/vecs.hpp"
#include "citygml/cityobject.h"


namespace plateau::geometry {

    /**
     * 緯度・経度・高さ による位置表現です。
     * PLATEAU の gmlファイルでは 緯度・経度・高さ によって座標が表現されます。
     *
     * 厳密にどの基準に基づいた座標であるかは、
     * GMLファイル名 または CityModel の Envelope の srcName に記載された
     * EPSGコードによって判別できます。
     * EPSGコードが 6697 のとき、それは
     * 「日本測地系2011における経緯度座標系と東京湾平均海面を基準とする標高の複合座標参照系」
     * になります。
     * EPSGコードが 10162 ～ 10174　の場合は平面直角座標系となります。
     */
    struct GeoCoordinate {
        double latitude;
        double longitude;
        double height;

        GeoCoordinate() = default;

        GeoCoordinate(double lat, double lon, double height) :
                latitude(lat),
                longitude(lon),
                height(height) {
        }

        GeoCoordinate operator+(GeoCoordinate op) const;
        GeoCoordinate operator*(double op) const;
        GeoCoordinate operator-(GeoCoordinate op) const;
        GeoCoordinate operator/(GeoCoordinate op) const;
    };


    /**
     * @enum CoordinateSystem
     *
     * 各列挙子について、3つのアルファベットはXYZ軸がどの方角、方向になるかを表しています。<br/>
     * N,S,E,Wはそれぞれ北,南,東,西<br/>
     * U,Dはそれぞれ上,下<br/>
     * に対応します。<br/>
     */
    enum class CoordinateSystem {
        //! PLATEAUでの座標系
        ENU = 0,
        WUN = 1,
        //! Unreal Engineでの座標系
        ESU = 2,
        //! Unityでの座標系
        EUN = 3
    };


    /**
     * 緯度・経度・高さの最小・最大で表現される範囲です。
     */
    struct Extent {
        GeoCoordinate min;
        GeoCoordinate max;

        Extent(const GeoCoordinate& min, const GeoCoordinate& max) {
            this->min = min;  // NOLINT(cppcoreguidelines-prefer-member-initializer)
            this->max = max;  // NOLINT(cppcoreguidelines-prefer-member-initializer)
        }

        bool contains(GeoCoordinate point, bool ignore_height = true) const;
        bool contains(TVec3d point, bool ignore_height = true) const;

        /**
         * 引数 city_obj の位置を推定し、その位置が Extent の範囲内に含まれるかどうかを返します。
         * city_obj の位置が不明の場合は false を返します。
         */
        bool contains(const citygml::CityObject& city_obj, bool ignore_height = true) const;

        /**
         * 平面直角座標系の判定を含む処理です
         * 平面直角座標の場合はunprojectして緯度経度に変換してから判定します。
         */
        bool containsInPolar(TVec3d point,const int epsg, bool ignore_height = true) const;
        bool containsInPolar(const citygml::CityObject& city_obj,const int epsg, bool ignore_height = true) const;

        /**
         * other と交わる箇所があるかどうかを返します。
         * ただし other の高さは無視して緯度と経度の2次元のみで判定します。
         */
        bool intersects2D(const Extent& other) const;

        /**
         * min と max の中点を GeoCoordinate で返します。
         */
        GeoCoordinate centerPoint() const;

        /// Extentの南西端のUVを(0,0),北東端のUVを(1,1)とするとき、指定位置のUVを求めます。
        TVec2f uvAt(GeoCoordinate coord) const;

        static Extent all() {
            return {
                    GeoCoordinate(-90, -180, -9999),
                    GeoCoordinate(90, 180, 9999)
            };
        }
    };

    /**
    * 平面直角座標判定、平面直角座標の基準点取得
    */
    struct CoordinateReferenceFactory {

        static constexpr int default_epsg = 6697;

        // EPSGごとのzone取得
        static int GetZoneId(int epsg) {
            // 日本測地系2011（JGD2011）に基づく平面直角座標系
            static const std::map<int, int> epsg_to_zone = {
                {10162, 1}, {10163, 2}, {10164, 3}, {10165, 4}, {10166, 5},
                {10167, 6}, {10168, 7}, {10169, 8}, {10170, 9}, {10171, 10},
                {10172, 11}, {10173, 12}, {10174, 13}
            };
            auto it = epsg_to_zone.find(epsg);
            return it != epsg_to_zone.end() ? it->second : 0;
        }

        // EPSGごとの基準点取得
        static GeoCoordinate GetReferencePoint(int epsg) {
            const int zone = GetZoneId(epsg);
            if (zone != 0)
                return GetReferencePointByZone(zone);
            return GeoCoordinate();
        }

        // Zone IDごとの基準点
        // zoneに紐づく基準点はPolarToPlaneCartesianにハードコードで持っているが値が取得できないので、ここで定義
        static GeoCoordinate GetReferencePointByZone(int zone_id) {
            static const std::map<int, GeoCoordinate> zone_to_point = {
                {1, GeoCoordinate(33, 129.5, 0)}, 
                {2, GeoCoordinate(33, 131, 0)},
                {3, GeoCoordinate(36, 132.166667, 0)}, 
                {4, GeoCoordinate(33, 133.5, 0)},
                {5, GeoCoordinate(36, 134.333333, 0)}, 
                {6, GeoCoordinate(36, 136, 0)},
                {7, GeoCoordinate(36, 137.166667, 0)}, 
                {8, GeoCoordinate(36, 138.5, 0)},
                {9, GeoCoordinate(35, 139.833333, 0)}, 
                {10, GeoCoordinate(40, 140.833333, 0)},
                {11, GeoCoordinate(44, 140.25, 0)}, 
                {12, GeoCoordinate(44, 142, 0)},
                {13, GeoCoordinate(43, 144, 0)}, 
                {14, GeoCoordinate(26, 142, 0)},
                {15, GeoCoordinate(26, 127.5, 0)}, 
                {16, GeoCoordinate(24, 124, 0)},
                {17, GeoCoordinate(31, 131, 0)}, 
                {18, GeoCoordinate(20, 136, 0)},
                {19, GeoCoordinate(25, 154, 0)}
            };
            auto it = zone_to_point.find(zone_id);
            return it != zone_to_point.end() ? it->second : GeoCoordinate();
        }

        // 極座標系・平面直角座標系判定
        static bool IsPolarCoordinateSystem(int epsg) {
            // 平面直角座標系の区分についてはこちらを参照してください :
            // https://www.mlit.go.jp/plateaudocument/toc9/toc9_08/toc9_08_04/
            if (epsg >= 10162 && epsg <= 10174) {
                return false;
            }
            return true;
        }
    };
}
