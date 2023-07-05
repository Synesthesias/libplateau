#include <plateau/polygon_mesh/node.h>
#include "plateau/polygon_mesh/city_object_list.h"
#include <algorithm>
#include <string>
#include <cassert>

namespace plateau::polygonMesh {

    CityObjectList& CityObjectList::operator=(CityObjectList&& city_object_list) {
        primary_id = city_object_list.primary_id;
        primary_atomic_id = city_object_list.primary_atomic_id;
        city_object_index_to_gml_id = std::move(city_object_list.city_object_index_to_gml_id);

        return *this;
    }

    CityObjectList::CityObjectList() {
    }

    CityObjectIndex CityObjectList::createPrimaryId() {
        primary_atomic_id = 0;
        return(CityObjectIndex(primary_id++, none));
    }

    CityObjectIndex CityObjectList::createPrimaryAtomicId() {
        return(CityObjectIndex(primary_id, primary_atomic_id++));
    }

    const std::string& CityObjectList::getAtomicGmlId(CityObjectIndex cityObjectIndex) {
        return(city_object_index_to_gml_id.at(cityObjectIndex));
    }

    const std::string& CityObjectList::getPrimaryGmlId(int id) {
        return(city_object_index_to_gml_id.at(CityObjectIndex(id, none)));
    }

    void CityObjectList::add(CityObjectIndex key, std::string value) {
        city_object_index_to_gml_id[key] = value;
    }
}
