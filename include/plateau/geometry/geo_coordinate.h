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

    struct ReferencePointFactory {
        // EPSGごとの基準点取得
        static void GetReferencePoint(double epsg, GeoCoordinate& point) {
            // EPSG:10169は、日本測地系2011（JGD2011）に基づく平面直角座標系第VIII系を指します。この座標系の基準点は、緯度36度、経度138.5度に設定されています。
            if (epsg == 10169) {
                point = GeoCoordinate(36, 138.5, 0);
                return;
            }
            else if (epsg == 10162) {
                point = GeoCoordinate(33, 129.5, 0);
                return;
            }
            else if (epsg == 10163) {
                point = GeoCoordinate(33, 131, 0);
                return;
            }
            point = GeoCoordinate();
        }

        static GeoCoordinate GetReferencePoint(double epsg) {
            // EPSG:10169は、日本測地系2011（JGD2011）に基づく平面直角座標系第VIII系を指します。この座標系の基準点は、緯度36度、経度138.5度に設定されています。
            if (epsg == 10169) {
                return GeoCoordinate(36, 138.5, 0);
            }
            else if (epsg == 10162) {
                return GeoCoordinate(33, 129.5, 0);
            }
            else if (epsg == 10163) {
                return GeoCoordinate(33, 131, 0);
            }
            return GeoCoordinate();
        }

        // 極座標系・平面直角座標系判定
        static bool IsPolarCoordinateSystem(double epsg) {
            // 平面直角座標系の区分についてはこちらを参照してください :
            // https://www.mlit.go.jp/plateaudocument/toc9/toc9_08/toc9_08_04/
            if (epsg >= 10162 && epsg <= 10174) {
                return false;
            }
            return true;
        }
    };
}
