#include <plateau/polygon_mesh/node.h>
#include "plateau/polygon_mesh/city_object_list.h"
#include <algorithm>
#include <string>
#include <cassert>

namespace plateau::polygonMesh {

    CityObjectList::CityObjectList() {
        auto size = city_object_index_to_gml_id.max_size();
    }

    static std::map<CityObjectIndex, std::string, std::less<>> feature_index_to_gml_id;

    CityObjectIndex CityObjectList::createtPrimaryId() {
        atomic_id = 0;
        return(CityObjectIndex(primary_id++, none));
    }

    CityObjectIndex CityObjectList::createtAtomicId() {
        return(CityObjectIndex(primary_id, atomic_id++));
    }

    const std::string& CityObjectList::getAtomicGmlId(CityObjectIndex featureIndex) {
        return(feature_index_to_gml_id[featureIndex]);
    }

    const std::string& CityObjectList::getPrimaryGmlId(int id) {
        return(feature_index_to_gml_id[CityObjectIndex(id, none)]);
    }

    void CityObjectList::add(CityObjectIndex key, std::string value) {
        
        feature_index_to_gml_id[key] = value;
    }
}
