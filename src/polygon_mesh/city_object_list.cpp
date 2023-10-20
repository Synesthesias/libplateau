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

    bool CityObjectList::tryGetPrimaryGmlID(int index, std::string& out_gml_id) const {
        auto key = CityObjectIndex(index, CityObjectIndex::invalidIndex());
        if(city_object_index_to_gml_id_.find(key) == city_object_index_to_gml_id_.end()){
            return false;
        }
        out_gml_id = getAtomicGmlID(key);
        return true;
    }

    bool CityObjectList::tryGetAtomicGmlID(const CityObjectIndex& city_obj_index, std::string& out_gml_id) const {
        if(!containsCityObjectIndex(city_obj_index)){
            return false;
        }
        out_gml_id = getAtomicGmlID(city_obj_index);
        return true;
    }

    bool CityObjectList::containsCityObjectIndex(const CityObjectIndex& city_obj_index) const {
        return !(city_object_index_to_gml_id_.find(city_obj_index) == city_object_index_to_gml_id_.end());
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

    std::vector<CityObjectIndex> CityObjectList::getAllPrimaryIndices() const {
        auto ret = std::vector<CityObjectIndex>();
        for(const auto& [index, _] : city_object_index_to_gml_id_) {
            if(index.atomic_index == CityObjectIndex::invalidIndex()) {
                ret.push_back(index);
            }
        }
        return ret;
    }

    std::vector<CityObjectIndex> CityObjectList::getAllAtomicIndices() const {
        auto ret = std::vector<CityObjectIndex>();
        for(const auto& [index, _] : city_object_index_to_gml_id_) {
            if(index.atomic_index != CityObjectIndex::invalidIndex()) {
                ret.push_back(index);
            }
        }
        return ret;
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

    bool CityObjectList::containsPrimaryGmlId(const std::string& primary_gml_id) const{
        const auto primaries = getAllPrimaryIndices();
        bool contains_primary = false;
        for(auto primary_index : primaries) {
            auto& gml_id = getPrimaryGmlID(primary_index.primary_index);
            if(gml_id == primary_gml_id){
                contains_primary = true;
                break;
            }
        }
        return contains_primary;
    }
}
