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

LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_polygon_get_uv1(
        const PlateauPolygon* const plateau_polygon,
        TVec2f* out_uvs
        ){
    API_TRY{
        auto& uv1 = plateau_polygon->getUV1();
        for(int i=0; i<uv1.size(); i++){
            out_uvs[i] = uv1.at(i);
        }
        return APIResult::Success;
    }
    API_CATCH;
    return APIResult::ErrorUnknown;
}
}