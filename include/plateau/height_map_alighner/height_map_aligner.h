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

        float posToHeight(TVec2d pos) const;
    };

    class LIBPLATEAU_EXPORT HeightMapAligner {
    public:
        void align(plateau::polygonMesh::Model& model, const HeightMapFrame& map);
    };
}
