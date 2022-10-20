#include <plateau/polygon_mesh/mesh_merger.h>
#include <cassert>
#include "libplateau_c.h"

using namespace libplateau;
using namespace plateau::polygonMesh;
using namespace plateau::geometry;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_merger_merge_mesh(
            Mesh* mesh,
            Mesh* other_mesh,
            CoordinateSystem mesh_axes,
            bool include_texture
    ) {
        API_TRY{
            MeshMerger::mergeMesh(
                    *mesh, *other_mesh, mesh_axes, include_texture,
                    TVec2f(0, 0), TVec2f(0, 0)
            );
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_merger_mesh_info(
            Mesh* mesh,
            TVec3d* vertices_array,
            int vertices_count,
            unsigned* indices_array,
            int indices_count,
            TVec2f* uv_1_array,
            int uv_1_count,
            CoordinateSystem mesh_axes,
            bool include_texture
    ) {
        API_TRY {
            auto vertices = std::vector<TVec3d>(vertices_array, vertices_array + vertices_count);
            auto indices = std::vector<unsigned>(indices_array, indices_array + indices_count);
            auto uv_1 = std::vector<TVec2f>(uv_1_array, uv_1_array + uv_1_count);
            MeshMerger::mergeMeshInfo(*mesh,
                                      std::move(vertices), std::move(indices), std::move(uv_1),
                                      mesh_axes, include_texture);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
