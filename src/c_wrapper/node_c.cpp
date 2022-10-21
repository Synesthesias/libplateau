#include "libplateau_c.h"
#include <plateau/polygon_mesh/node.h>
#include <plateau/polygon_mesh/mesh.h>
using namespace libplateau;
using namespace plateau::polygonMesh;
extern "C" {

LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_node(
        Node** out_node_ptr,
        char* id
){
    *out_node_ptr = new Node(std::string(id));
    return APIResult::Success;
}

LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_node(
        Node* node_ptr
){
    delete node_ptr;
    return APIResult::Success;
}

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

    /**
     * Node に Mesh をセットします。
     * 取扱注意:
     * std::move により Mesh を移動するので、前の Mesh は利用不可になります。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_node_set_mesh_by_std_move(
            Node* node,
            Mesh* mesh
            ){
    API_TRY{
        node->setMesh(std::move(*mesh));
        return APIResult::Success;
    }API_CATCH;
    return APIResult::ErrorUnknown;
}
}
