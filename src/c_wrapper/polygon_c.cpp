#include <citygml/polygon.h>

#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {
DLL_VALUE_FUNC(plateau_polygon_get_vertex_count,
               Polygon,
               int,
               handle->getVertices().size())

DLL_VALUE_FUNC_WITH_INDEX_CHECK(plateau_polygon_get_vertex,
               Polygon,
               TVec3d,
               handle->getVertices().at(index),
               index >= handle->getVertices().size())

DLL_VALUE_FUNC(plateau_polygon_get_indices_count,
              Polygon,
              int,
              handle->getIndices().size())

DLL_VALUE_FUNC_WITH_INDEX_CHECK(plateau_polygon_get_index_of_indices,
               Polygon,
               int,
               handle->getIndices().at(index),
               index >= handle->getIndices().size())

DLL_PTR_FUNC(plateau_polygon_get_exterior_ring,
             Polygon,
             LinearRing,
             handle->exteriorRing().get())

DLL_VALUE_FUNC(plateau_polygon_get_interior_ring_count,
            Polygon,
            int,
            handle->interiorRings().size())

DLL_PTR_FUNC(plateau_polygon_get_interior_ring,
             Polygon,
             LinearRing,
             handle->interiorRings().at(index).get(),
             ,int index)
}