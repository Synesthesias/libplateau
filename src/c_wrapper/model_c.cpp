#include "libplateau_c.h"
#include <plateau/geometry/model.h>
using namespace libplateau;
using namespace plateau::geometry;
extern "C" {

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_model_new(
            Model** out_model_ptr
            ){
        *out_model_ptr = new Model();
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_model_delete(
            Model* model_ptr
            ){
        delete model_ptr;
        return APIResult::Success;
    }

    DLL_VALUE_FUNC(plateau_model_get_root_nodes_count,
                   Model,
                   int,
                   handle->getRootNodesCount())

    // TODO 範囲外のindexが渡されたら APIResult で知らせる方が安全
    DLL_PTR_FUNC(plateau_model_get_root_node_at_index,
                 Model,
                 Node,
                 &handle->getRootNodeAt(index),
                 ,int index)
}