#include <citygml/linearring.h>

#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

DLL_VALUE_FUNC(plateau_linear_ring_get_vertices_count,
               LinearRing,
               int,
               handle->size())

DLL_VALUE_FUNC(plateau_linear_ring_get_vertex,
               LinearRing,
               TVec3d,
               handle->getVertices().at(index),
               ,int index)

}