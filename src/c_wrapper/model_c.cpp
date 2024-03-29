#include "libplateau_c.h"
#include <plateau/polygon_mesh/model.h>
using namespace libplateau;
using namespace plateau::polygonMesh;
extern "C" {

    DLL_CREATE_FUNC(plateau_create_model,
                    Model)

    DLL_DELETE_FUNC(plateau_delete_model,
                    Model)

    DLL_VALUE_FUNC(plateau_model_get_root_nodes_count,
                   Model,
                   int,
                   handle->getRootNodeCount())

    DLL_PTR_FUNC_WITH_INDEX_CHECK(plateau_model_get_root_node_at_index,
                 Model,
                 Node,
                 &handle->getRootNodeAt(index),
                 index >= handle->getRootNodeCount())

    /// node を model::root_node_ に加えます。
    /// std::move によって node を移動するので、移動後に元の Node は利用不可になります。
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_model_add_node_by_std_move(
            Model* model,
            Node* node
    ) {
        API_TRY {
            model->addNode(std::move(*node));
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
