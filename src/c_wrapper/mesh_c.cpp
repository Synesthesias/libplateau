#include <plateau/polygon_mesh/mesh.h>
#include "libplateau_c.h"
#include <cassert>
using namespace citygml;
using namespace libplateau;
using namespace plateau::polygonMesh;
extern "C"{

    DLL_CREATE_FUNC(plateau_create_mesh,
                    Mesh)

    DLL_DELETE_FUNC(plateau_delete_mesh,
                    Mesh)

    DLL_VALUE_FUNC(plateau_mesh_get_vertices_count,
                       Mesh,
                       int,
                       handle->getVertices().size())

    DLL_VALUE_FUNC_WITH_INDEX_CHECK(plateau_mesh_get_vertex_at_index,
                                    Mesh,
                                    TVec3d,
                                    handle->getVertices().at(index),
                                    index >= handle->getVertices().size())

    DLL_VALUE_FUNC(plateau_mesh_get_indices_count,
                  Mesh,
                  int,
                  handle->getIndices().size())

    DLL_VALUE_FUNC_WITH_INDEX_CHECK(plateau_mesh_get_indice_at_index,
                 Mesh,
                 int,
                 handle->getIndices().at(index),
                 index >= handle->getIndices().size())

    DLL_VALUE_FUNC(plateau_mesh_get_sub_mesh_count,
                   Mesh,
                   int,
                   handle->getSubMeshes().size())

    DLL_PTR_FUNC_WITH_INDEX_CHECK(plateau_mesh_get_sub_mesh_at_index,
                 Mesh,
                 SubMesh,
                 &handle->getSubMeshes().at(index),
                 index >= handle->getSubMeshes().size())

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_add_sub_mesh(
            Mesh* mesh,
            char* texture_path,
            int sub_mesh_start_index,
            int sub_mesh_end_index
    ) {
        API_TRY {
            mesh->addSubMesh(texture_path, sub_mesh_start_index, sub_mesh_end_index);
            return APIResult::Success;
        } API_CATCH
        return APIResult::ErrorUnknown;
    }

    DLL_PTR_FUNC(plateau_mesh_get_city_object_list,
                 Mesh,
                 CityObjectList,
                 &handle->getCityObjectList())

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
    // UV4 を取得する関数
    PLATEAU_MESH_GET_UV(4)
}
