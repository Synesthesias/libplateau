#include "libplateau_c.h"
#include <plateau/polygon_mesh/mesh_extract_options.h>
extern "C" {
    using namespace libplateau;
    using namespace plateau::polygonMesh;

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extract_options_default_value(MeshExtractOptions* out_default_options){
        API_TRY{
            *out_default_options = MeshExtractOptions();
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
