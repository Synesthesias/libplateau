#include <citygml/polygon.h>

#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {
DLL_VALUE_FUNC(plateau_polygon_get_vertices_count,
               Polygon,
               int,
               handle->getVertices().size())

DLL_VALUE_FUNC(plateau_polygon_get_vertex,
               Polygon,
               TVec3d,
               handle->getVertices().at(index),
               ,int index)

DLL_VALUE_FUNC(plateau_polygon_get_indices_count,
              Polygon,
              int,
              handle->getIndices().size())

DLL_VALUE_FUNC(plateau_polygon_get_index_of_indices,
               Polygon,
               int,
               handle->getIndices().at(index_of_indices_list),
               ,int index_of_indices_list)
}