#include "libplateau_c.h"
#include <plateau/geometry/node.h>
using namespace libplateau;
using namespace plateau::geometry;
extern "C" {

DLL_STRING_PTR_FUNC(plateau_node_get_name,
                    Node,
                    handle->getName())

DLL_VALUE_FUNC(plateau_node_get_child_count,
               Node,
               int,
               handle->getChildCount())

DLL_PTR_FUNC(plateau_node_get_child_at_index,
            Node,
            Node,
            &handle->getChildAt(index),
            ,int index)
}