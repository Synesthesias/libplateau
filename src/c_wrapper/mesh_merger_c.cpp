#include "libplateau_c.h"

#include <plateau/polygon_mesh/mesh_merger.h>

using namespace libplateau;
using namespace plateau::polygonMesh;
using namespace plateau::geometry;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_merger_merge_mesh(
            Mesh* mesh,
            const Mesh* other_mesh,
            const bool invert_mesh_front_back,
            const bool include_texture
    ) {
        API_TRY{
            MeshMerger::mergeMesh(*mesh, *other_mesh, invert_mesh_front_back, include_texture);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
