#pragma once
#include <libplateau_api.h>
#include "plateau/polygon_mesh/model.h"

namespace plateau::heightMapAligner {

    /// 高さマップと、そこから高さを導くためのmin,max等の情報をまとめてHeightMapFrameと名付けます。
    class HeightMapFrame {
    public:
        HeightMapFrame(std::vector<uint16_t> heightmap, int map_width, int map_height, float min_x, float max_x,
                       float min_y, float max_y, float min_height, float max_height) :
                heightmap(std::move(heightmap)), map_width(map_width), map_height(map_height), min_x(min_x), max_x(max_x), max_y(max_y), min_y(min_y), min_height(min_height), max_height(max_height)
        {}
        std::vector<uint16_t> heightmap;
        int map_width;
        int map_height;
        float min_x;
        float max_x;
        float min_y;
        float max_y;
        float min_height;
        float max_height;

        /// 場所から、ハイトマップ上の高さを返します。
        /// offset_mapは、ハイトマップの値でどれだけずらすかを指定します。
        float posToHeight(TVec2d pos, double height_offset) const;
    };

    /// モデルの高さを、高さマップの高さに合わせます。
    /// 使い方は、合わせたい高さマップを addHeightmapFrame で追加し、最後に Align で合わせます。
    /// 地形が複数ある場合を想定し、合わせたい高さマップが複数ある場合は addHeightmapFrame をその数の回数だけ呼び出してください。
    /// 複数ある場合、位置が合う高さマップが利用されます。
    class LIBPLATEAU_EXPORT HeightMapAligner {
    public:

        /// コンストラクタで高さのオフセットを指定します。
        explicit HeightMapAligner(double height_offset) : height_offset(height_offset) {}

        void addHeightmapFrame(const HeightMapFrame& heightmap_frame);
        void align(plateau::polygonMesh::Model& model) const;
        int heightmapCount() const { return height_map_frames.size(); }
    private:
        std::vector<HeightMapFrame> height_map_frames;
        const double height_offset;
    };
}
