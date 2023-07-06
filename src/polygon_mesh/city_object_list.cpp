#include <plateau/polygon_mesh/node.h>
#include "plateau/polygon_mesh/city_object_list.h"
#include <string>

namespace plateau::polygonMesh {
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
}
