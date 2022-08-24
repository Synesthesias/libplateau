#include <plateau/geometry/mesh.h>
#include "libplateau_c.h"
using namespace citygml;
using namespace libplateau;
using namespace plateau::geometry;
extern "C"{
DLL_VALUE_FUNC(plateau_mesh_get_multi_texture_count,
               Mesh,
               int,
               handle->getMultiTexture().size())


LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_get_multi_texture(
        const Mesh* const plateau_mesh,
        int* const out_vertex_index_array,
        const char** const out_url_str_pointers,
        int* const out_str_lengths
        ) {
    API_TRY {
        auto& multiTex = plateau_mesh->getMultiTexture();
        int i = 0;
        for(auto& indexToTex : multiTex){
            int index = indexToTex.first;
            auto& tex_url = indexToTex.second;
            out_vertex_index_array[i] = index;
            out_url_str_pointers[i] = tex_url.c_str();
            out_str_lengths[i] = (int)tex_url.length() + 1; // +1 はnull終端文字の分
            i++;
        }
        return APIResult::Success;
    }
    API_CATCH;
    return APIResult::ErrorUnknown;
}

/**
 * UVを取得するマクロです。
 */
#define PLATEAU_MESH_GET_UV(UV_INDEX)\
LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_get_uv ## UV_INDEX  ( \
        const Mesh* const mesh, \
        TVec2f* out_uvs \
){ \
    API_TRY{ \
        auto& uv = mesh->getUV ## UV_INDEX (); \
        for(int i=0; i<uv.size(); i++){ \
            out_uvs[i] = uv.at(i); \
        } \
        return APIResult::Success; \
    } \
    API_CATCH; \
    return APIResult::ErrorUnknown; \
}

// UV1 を取得する関数
PLATEAU_MESH_GET_UV(1)
// UV2 を取得する関数
PLATEAU_MESH_GET_UV(2)
// UV3 を取得する関数
PLATEAU_MESH_GET_UV(3)
}