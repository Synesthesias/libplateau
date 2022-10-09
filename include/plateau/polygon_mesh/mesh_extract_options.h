#pragma once

#include <plateau/io/mesh_convert_options.h>
#include <citygml/vecs.hpp>

namespace plateau::polygonMesh {

    struct MeshExtractOptions {
        MeshExtractOptions(TVec3d reference_point, CoordinateSystem mesh_axes, MeshGranularity mesh_granularity,
                           unsigned max_lod, unsigned min_lod, bool export_appearance, int grid_count_of_side,
                           float unit_scale, int coordinate_zone_id,
                           bool exclude_city_obj_if_first_vertex_is_outside_extent,
                           bool exclude_triangles_outside_extent, Extent extent) :
                reference_point(reference_point),
                mesh_axes(mesh_axes),
                mesh_granularity(mesh_granularity),
                max_lod(max_lod),
                min_lod(min_lod),
                export_appearance(export_appearance),
                grid_count_of_side(grid_count_of_side),
                unit_scale(unit_scale),
                coordinate_zone_id(coordinate_zone_id),
                exclude_city_obj_if_first_vertex_is_outside_extent(exclude_city_obj_if_first_vertex_is_outside_extent),
                exclude_triangles_outside_extent(exclude_triangles_outside_extent),
                extent(extent){}

    public:
        TVec3d reference_point;
        CoordinateSystem mesh_axes;
        MeshGranularity mesh_granularity;
        unsigned max_lod;
        unsigned min_lod;
        bool export_appearance;
        /**
         * グリッド分けする時の、1辺の分割数です。
         * この数の2乗がグリッドの数となり、実際にはそれより細かくグループ分けされます。
         */
        int grid_count_of_side;
        float unit_scale;

        /**
         * 国土交通省が規定する、日本における平面直角座標系の基準点のうちどれを採用するかを番号で指定します。
         * 基準点と番号は次のWebサイトに記載のとおりです。
         * https://www.gsi.go.jp/sokuchikijun/jpc.html
         */
        int coordinate_zone_id;


        /**
         * 範囲外の3Dモデルを出力から除外するための、2つの方法のうち1つを有効にするかどうかを bool で指定します。
         * その方法とは、都市オブジェクトの最初の頂点の位置が範囲外のとき、そのオブジェクトはすべて範囲外とみなして出力から除外します。
         * これはビル1棟程度の大きさのオブジェクトでは有効ですが、
         * 10km×10kmの地形のような巨大なオブジェクトでは、実際には範囲内なのに最初の頂点が遠いために除外されるということがおきます。
         * したがって、この値は建物では true, 地形では false となるべきです。
         */
        bool exclude_city_obj_if_first_vertex_is_outside_extent;

        /**
         * 範囲外の3Dモデルを出力から除外するための、2つの方法のうち1つを有効にするかどうかを bool で指定します。
         * その方法とは、メッシュ操作によって、範囲外に存在するポリゴンを除外します。
         * この方法であれば 10km×10km の地形など巨大なオブジェクトにも対応できます。
         */
        bool exclude_triangles_outside_extent;


        Extent extent;
    };
}
