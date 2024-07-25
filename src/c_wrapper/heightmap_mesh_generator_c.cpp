#include <plateau/height_map_generator/heightmap_mesh_generator.h>
#include "libplateau_c.h"
#include <plateau/polygon_mesh/mesh.h>
#include <cstring>

#include "plateau/height_map_generator/heightmap_types.h"

using namespace citygml;
using namespace libplateau;
using namespace plateau::polygonMesh;
using namespace plateau::heightMapGenerator;
using namespace plateau::geometry;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_heightmap_mesh_generator_generate(
        const int texture_width,
        const int texture_height,
        const float z_scale,
        const HeightMapElemT* data,
        const CoordinateSystem coordinate,
        const TVec3d min,
        const TVec3d max,
        const float uv_min_x,
        const float uv_min_y,
        const float uv_max_x,
        const float uv_max_y,
        const bool invert_mesh,
        Mesh* const out_mesh
    ) {
        API_TRY{
            HeightmapMeshGenerator generator;
            generator.generateMeshFromHeightmap(
                *out_mesh, texture_width, texture_height, z_scale, data, coordinate, min, max, { uv_min_x, uv_min_y }, {uv_max_x, uv_max_y }, invert_mesh
            );
            return APIResult::Success;
        } 
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
