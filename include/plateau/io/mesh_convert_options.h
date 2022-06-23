#pragma once

/**
 * \brief 座標系
 *
 * 各列挙子について、3つのアルファベットはXYZ軸がどの方角、方向になるかを表しています。<br/>
 * N,S,E,Wはそれぞれ北,南,東,西<br/>
 * U,D,L,R,F,Bはそれぞれ上,下,左,右,前,後<br/>
 * に対応します。<br/>
 */
enum class AxesConversion {
    WNU,
    RUF
};

/**
 * \brief メッシュの結合単位
 */
enum class MeshGranularity {
    /**
     * \brief 最小地物単位(LOD2, LOD3の各部品)
     */
    PerAtomicFeatureObject,
    /**
     * \brief 主要地物単位(建築物、道路等)
     */
     PerPrimaryFeatureObject,
     /**
      * \brief 都市モデル地域単位(GMLファイル内のすべてを結合)
      */
      PerCityModelArea
};

/**
 * \brief 都市モデルからメッシュへの変換設定です。
 */
struct MeshConvertOptions {
    /**
     * \brief 出力後のメッシュの座標系を指定します。
     */
    AxesConversion mesh_axes;

    /**
     * \brief 変換時の基準点を指定します。
     *
     * 基準点は平面直角座標であり、メッシュは基準点からの相対座標で出力されます。
     */
    TVec3d reference_point;

    /**
     * \brief 出力後のメッシュの粒度(結合単位)を指定します。
     */
    MeshGranularity mesh_granularity;

    /**
     * \brief 出力後のメッシュに含める最小のLODを指定します。
     */
    unsigned min_lod;

    /**
     * \brief 出力後のメッシュに含める最大のLODを指定します。
     */
    unsigned max_lod;

    /**
     * \brief 1つの地物について複数のLODがある場合に最大LOD以外のジオメトリを出力するかどうかを指定します。
     */
    bool export_lower_lod;

    /**
     * \brief テクスチャ、マテリアル情報を出力するかどうかを指定します。
     */
    bool export_appearance;

    /**
     * \brief 緯度経度からメートルへの変換を行うかどうかを指定します。
     */
    bool convert_lat_lon;

    MeshConvertOptions() :
        mesh_axes(AxesConversion::RUF),
        reference_point(),
        mesh_granularity(MeshGranularity::PerPrimaryFeatureObject),
        min_lod(0),
        max_lod(3),
        export_lower_lod(true),
        export_appearance(true),
        convert_lat_lon(true) {
    }
};
