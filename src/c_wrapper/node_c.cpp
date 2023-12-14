#include "libplateau_c.h"
#include <plateau/polygon_mesh/node.h>
#include <plateau/polygon_mesh/mesh.h>
using namespace libplateau;
using namespace plateau::polygonMesh;
extern "C" {

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_node(
            Node** out_node_ptr,
            char* id
    ) {
        *out_node_ptr = new Node(id);
        return APIResult::Success;
    }

    DLL_DELETE_FUNC(plateau_delete_node,
                    Node)

    DLL_STRING_PTR_FUNC(plateau_node_get_name,
                        Node,
                        handle->getName())

    DLL_VALUE_FUNC(plateau_node_get_child_count,
                   Node,
                   int,
                   handle->getChildCount())

   DLL_VALUE_FUNC(plateau_node_get_is_active,
                  Node,
                  bool,
                  handle->isActive())

    DLL_2_ARG_FUNC(plateau_node_set_is_active,
                   Node* const node,
                   const bool is_active,
                   node->setIsActive(is_active))

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
            const auto mesh = node->getMesh();
            if (mesh != nullptr) {
                *out_mesh_ptr = mesh;
            } else {
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
            ) {
        API_TRY {
            node->setMesh(std::unique_ptr<Mesh>(mesh));
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    /**
     * 取扱注意:
     * move するので、 前の child_node は利用不可になります。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_node_add_child_node_by_std_move(
            Node* node,
            Node* child_node
    ) {
        API_TRY {
            node->addChildNode(std::move(*child_node));
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
