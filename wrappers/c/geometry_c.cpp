
#include <citygml/geometry.h>

#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C"{

DLL_VALUE_FUNC(plateau_geometry_get_type,
               Geometry,
               Geometry::GeometryType,
               handle->getType())

DLL_VALUE_FUNC(plateau_geometry_get_geometries_count,
               Geometry,
               int,
               handle->getGeometriesCount())

DLL_VALUE_FUNC(plateau_geometry_get_polygons_count,
               Geometry,
               int,
               handle->getPolygonsCount())

DLL_PTR_FUNC(plateau_geometry_get_child_geometry,
               Geometry,
               Geometry,
               &handle->getGeometry(index),
               ,int index)

DLL_PTR_FUNC(plateau_geometry_get_polygon,
             Geometry,
             Polygon,
             handle->getPolygon(index).get(),
             ,int index)

DLL_VALUE_FUNC(plateau_geometry_get_lod,
             Geometry,
             int,
             handle->lod())

DLL_STRING_PTR_FUNC(plateau_geometry_get_srs_name,
                    Geometry,
                    handle->getSRSName())

DLL_VALUE_FUNC(plateau_geometry_get_line_string_count,
               Geometry,
               int,
               handle->getLineStringCount())
}