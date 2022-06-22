#include <iostream>
#include <citygml/cityobject.h>

#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {

    DLL_VALUE_FUNC(plateau_city_object_get_type,
                   CityObject,
                   CityObject::CityObjectsType,
                   handle->getType())

    DLL_VALUE_FUNC(plateau_city_object_get_geometries_count,
                   CityObject,
                   int,
                   handle->getGeometriesCount())

    DLL_PTR_FUNC(plateau_city_object_get_address,
                 CityObject, Address,
                 handle->address())

    DLL_VALUE_FUNC(plateau_city_object_get_implicit_geometry_count,
                   CityObject,
                   int,
                   handle->getImplicitGeometryCount())

    DLL_VALUE_FUNC(plateau_city_object_get_child_city_object_count,
                   CityObject,
                   int,
                   handle->getChildCityObjectsCount())

    DLL_PTR_FUNC(plateau_city_object_get_child_city_object,
                 CityObject,
                 CityObject,
                 &handle->getChildCityObject(index),
                 ,int index)

    DLL_PTR_FUNC(plateau_city_object_get_geometry,
                 CityObject,
                 Geometry,
                 &handle->getGeometry(index),
                 ,int index)
}