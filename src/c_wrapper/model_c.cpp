#include "libplateau_c.h"
#include <plateau/polygon_mesh/model.h>
using namespace libplateau;
using namespace plateau::polygonMesh;
extern "C" {

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_model(
            Model** out_model_ptr
            ){
        *out_model_ptr = new Model();
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_model(
            Model* model_ptr
            ){
        delete model_ptr;
        return APIResult::Success;
    }

    DLL_VALUE_FUNC(plateau_model_get_root_nodes_count,
                   Model,
                   int,
                   handle->getRootNodeCount())

    DLL_PTR_FUNC_WITH_INDEX_CHECK(plateau_model_get_root_node_at_index,
                 Model,
                 Node,
                 &handle->getRootNodeAt(index),
                 index >= handle->getRootNodeCount())
}
