#include <plateau/polygon_mesh/node.h>
#include "plateau/polygon_mesh/city_object_list.h"
#include <string>

namespace plateau::polygonMesh {

    CityObjectList::CityObjectList(const std::vector<std::tuple<CityObjectIndex, std::string>>& initial_val) {
        for(const auto& [city_obj_index, gml_id] : initial_val) {
            city_object_index_to_gml_id_[city_obj_index] = gml_id;
        }
    };

    const std::string& CityObjectList::getAtomicGmlID(const CityObjectIndex& city_object_index) const {
        return city_object_index_to_gml_id_.at(city_object_index);
    }

    const std::string& CityObjectList::getPrimaryGmlID(const int index) const {
        return city_object_index_to_gml_id_.at({ index, CityObjectIndex::invalidIndex() });
    }

    void CityObjectList::getAllKeys(std::vector<CityObjectIndex>& keys) const {
        for (const auto& [key, _] : city_object_index_to_gml_id_) {
            keys.push_back(key);
        }
    }

    std::shared_ptr<std::vector<CityObjectIndex>> CityObjectList::getAllKeys() const {
        auto result = std::make_shared<std::vector<CityObjectIndex>>();
        getAllKeys(*result);
        return result;
    }

    CityObjectIndex CityObjectList::getCityObjectIndex(const std::string& gml_id) const {
        for (const auto& [key, value] : city_object_index_to_gml_id_) {
            if (value == gml_id)
                return key;
        }
        return { -1, -1 };
    }

    void CityObjectList::add(const CityObjectIndex& key, const std::string& value) {
        city_object_index_to_gml_id_[key] = value;
    }

    size_t CityObjectList::size() const {
        return city_object_index_to_gml_id_.size();
    }

    bool CityObjectList::operator==(const CityObjectList& other) const {
        if(size() != other.size()) return false;
        auto other_iter = other.city_object_index_to_gml_id_.begin();
        for(const auto& this_pair : city_object_index_to_gml_id_){
            const auto& other_pair = *other_iter;
            if(this_pair != other_pair) return false;
            other_iter++;
        }
        return true;
    }
}
