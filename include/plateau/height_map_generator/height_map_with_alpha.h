#pragma once

#include <libplateau_api.h>
#include <plateau/height_map_generator/heightmap_types.h>

namespace plateau::heightMapGenerator {

    /// ハイトマップとアルファマップをまとめたクラスです。
    class LIBPLATEAU_EXPORT HeightMapWithAlpha {
    public:
        HeightMapWithAlpha(size_t texture_width, size_t texture_height, double cartesian_width, double cartesian_height);
        void setHeightAt(size_t index, HeightMapElemT height);
        HeightMapElemT getHeightAt(size_t index) const;
        void setAlphaAt(size_t index, AlphaMapElemT alpha);

        /// アルファが0に近いときtrue
        bool isInvisibleAt(size_t index) const;

        /// アルファが1に近いときtrue
        bool isOpaqueAt(size_t index) const;
        AlphaMapElemT getAlphaAt(size_t index) const;

        /// 透明部分をエッジ色でfillします。
        void fillTransparentEdges();

        /// ハイトマップを平滑化します。手法は3×3ピクセルの平均値にならします。
        void applyConvolutionFilterForHeightMap();

        /// アルファマップの不透明部分を広げます。
        void expandOpaqueArea();

        /// アルファマップを平滑化します。手法は一定距離内の平均値にならします。
        void averagingAlphaMap();

        HeightMapT getHeightMap();
        int indexAt(int x, int y) const;

    private:
        HeightMapT height_map;
        AlphaMapT alpha_map;
        size_t texture_width;
        size_t texture_height;
        double cartesian_width; // テクスチャマップの右から左までの、直交座標系の距離
        double cartesian_height; // テクスチャマップの下から上までの、直交座標系の距離
    };
}
