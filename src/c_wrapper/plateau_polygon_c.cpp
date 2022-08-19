#include <plateau/io/plateau_polygon.h>
#include "libplateau_c.h"
using namespace citygml;
using namespace libplateau;

extern "C"{
DLL_VALUE_FUNC(plateau_polygon_get_multi_texture_count,
               PlateauPolygon,
               int,
               handle->getMultiTexture().size())

LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_polygon_get_multi_texture(
        const PlateauPolygon* const plateau_polygon,
        int* const out_vertex_index_array,
        const Texture** out_texture_array
        ) {
    API_TRY {
        auto multiTex = plateau_polygon->getMultiTexture();
        int i = 0;
        for(auto& indexToTex : multiTex){
            int index = indexToTex.first;
            auto tex = indexToTex.second;
            out_vertex_index_array[i] = index;
            out_texture_array[i] = tex.get();
            i++;
        }
        return APIResult::Success;
    }
    API_CATCH;
    return APIResult::ErrorUnknown;
}

#define PLATEAU_POLYGON_GET_UV(UV_INDEX)\
LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_polygon_get_uv ## UV_INDEX  ( \
        const PlateauPolygon* const plateau_polygon, \
        TVec2f* out_uvs \
){ \
    API_TRY{ \
        auto& uv = plateau_polygon->getUV ## UV_INDEX (); \
        for(int i=0; i<uv.size(); i++){ \
            out_uvs[i] = uv.at(i); \
        } \
        return APIResult::Success; \
    } \
    API_CATCH; \
    return APIResult::ErrorUnknown; \
}

// UV1 を取得する関数
PLATEAU_POLYGON_GET_UV(1)
// UV2 を取得する関数
PLATEAU_POLYGON_GET_UV(2)
// UV3 を取得する関数
PLATEAU_POLYGON_GET_UV(3)
}