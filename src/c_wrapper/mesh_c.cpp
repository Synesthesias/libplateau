#include <plateau/polygon_mesh/mesh.h>
#include "libplateau_c.h"
#include <cassert>
using namespace citygml;
using namespace libplateau;
using namespace plateau::polygonMesh;
extern "C" {

    DLL_CREATE_FUNC(plateau_create_mesh,
                    Mesh)

        LIBPLATEAU_C_EXPORT libplateau::APIResult LIBPLATEAU_C_API plateau_create_mesh_9(
            Mesh** out_ptr,
            TVec3d* vertices_array,
            const int vertices_count,
            unsigned* indices_array,
            const int indices_count,
            TVec2f* uv_1_array,
            const int uv_1_count,
            TVec2f* uv_4_array,
            const int uv_4_count,
            SubMesh** sub_mesh_pointers_array,
            const int sub_mesh_count
        ) {
            API_TRY {
                auto vertices = std::vector<TVec3d>(vertices_array, vertices_array + vertices_count);
                auto indices = std::vector<unsigned>(indices_array, indices_array + indices_count);
                auto uv_1 = std::vector<TVec2f>(uv_1_array, uv_1_array + uv_1_count);
                auto uv_4 = std::vector<TVec2f>(uv_4_array, uv_4_array + uv_4_count);

                auto sub_meshes = std::vector<SubMesh>();
                sub_meshes.reserve(sub_mesh_count);
                for (int i = 0; i < sub_mesh_count; i++) {
                    const SubMesh* sub_mesh_ptr = *(sub_mesh_pointers_array + i);
                    sub_meshes.push_back(*sub_mesh_ptr);
                }
                
            *out_ptr = new Mesh(std::move(vertices), std::move(indices), std::move(uv_1), std::move(uv_4), std::move(sub_meshes), CityObjectList());
            return libplateau::APIResult::Success;
        }API_CATCH
            return libplateau::APIResult::ErrorUnknown;
    }

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
        API_TRY{
            mesh->addSubMesh(texture_path, nullptr, sub_mesh_start_index, sub_mesh_end_index, -1);
            return APIResult::Success;
        } API_CATCH
            return APIResult::ErrorUnknown;
    }


    DLL_VALUE_FUNC(plateau_mesh_get_vertex_color_count,
                   Mesh,
                   int,
                   handle->getVertexColors().size())

    DLL_VALUE_FUNC_WITH_INDEX_CHECK(plateau_mesh_get_vertex_color_at_index,
                                   Mesh,
                                   TVec3d,
                                   handle->getVertexColors().at(index),
                                   index >= handle->getVertexColors().size())

   LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_set_vertex_colors(
           Mesh* mesh,
           TVec3d* color_array,
           int color_array_count
           ) {
        API_TRY {
            auto colors = std::vector<TVec3d>(color_array, color_array + color_array_count);
            mesh->setVertexColors(colors);
            return APIResult::Success;
        } API_CATCH
            return APIResult::ErrorUnknown;
    }


    DLL_PTR_FUNC(plateau_mesh_get_city_object_list,
                 Mesh,
                 CityObjectList,
                 &handle->getCityObjectList())

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_set_city_object_list(
            Mesh* const mesh,
            const CityObjectList* const city_obj_list
    ) {
        API_TRY {
            mesh->setCityObjectList(*city_obj_list);
            return APIResult::Success;
        }
        API_CATCH
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
        // UV4 を取得する関数
        LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_get_uv4(const Mesh* const mesh, TVec2f* out_uvs) {
        try {
            auto& uv = mesh->getUV4();
            for (int i = 0; i < uv.size(); i++) {
                out_uvs[i] = uv.at(i);
            } return APIResult::Success;
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occurred." << std::endl;
        }; return APIResult::ErrorUnknown;
    }
}
