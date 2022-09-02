#pragma once

namespace plateau::geometry{

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
     * TODO
     * EPSGコードの判別と、それによって処理を変える機能は未実装です。
     */
    struct GeoCoordinate{
    public:
        double latitude;
        double longitude;
        double height;

        GeoCoordinate(double lat, double lon, double height_arg){
            latitude = lat;
            longitude = lon;
            height = height_arg;
        }
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
        ENU,
        //! Unityでの座標系
        WUN,
        //! Unreal Engineでの座標系
        NWU
    };
}
