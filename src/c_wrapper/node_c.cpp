#include "libplateau_c.h"
#include <plateau/geometry/node.h>
#include <plateau/geometry/mesh.h>
using namespace libplateau;
using namespace plateau::polygonMesh;
extern "C" {

    DLL_STRING_PTR_FUNC(plateau_node_get_name,
                        Node,
                        handle->getName())

    DLL_VALUE_FUNC(plateau_node_get_child_count,
                   Node,
                   int,
                   handle->getChildCount())

    DLL_PTR_FUNC_WITH_INDEX_CHECK(plateau_node_get_child_at_index,
                Node,
                Node,
                &handle->getChildAt(index),
                index >= handle->getChildCount())

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_node_get_mesh(
            Node* node,
            Mesh** out_mesh_ptr
            ) {
        API_TRY{
            auto& mesh = node->getMesh();
            if(mesh.has_value()){
                *out_mesh_ptr = &mesh.value();
            }else{
                return APIResult::ErrorValueNotFound;
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
