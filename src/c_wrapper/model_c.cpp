#include "libplateau_c.h"
#include <plateau/geometry/model.h>
using namespace libplateau;
using namespace plateau::geometry;
extern "C" {

DLL_VALUE_FUNC(plateau_model_get_root_nodes_count,
               Model,
               int,
               handle->getRootNodesCount())

DLL_PTR_FUNC(plateau_model_get_root_node_at_index,
             Model,
             Node,
             &handle->getRootNodeAt(index),
             ,int index)
}