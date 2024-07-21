#include <plateau/material_adjust/material_adjuster_by_type.h>

using namespace citygml;
namespace plateau::materialAdjust {

    bool MaterialAdjusterByType::registerType(const std::string& gml_id,
                                              const CityObject::CityObjectsType type) {
        auto result = gml_id_to_type.insert({gml_id, type});
        return result.second;
    }

    bool MaterialAdjusterByType::registerMaterialPattern(const CityObject::CityObjectsType type,
                                                         const int game_mat_id) {
        auto result = type_to_game_mat_id.insert({type, game_mat_id});
        return result.second;
    }

    void MaterialAdjusterByType::exec(plateau::polygonMesh::Model& model) {
        auto materialAdjusterCommon = MaterialAdjusterCommon(this);
        materialAdjusterCommon.exec(model);
    }

    bool MaterialAdjusterByType::shouldOverrideMaterial(const std::string& gml_id,
                                                        int& out_next_override_game_mat_id) const {

        bool ret = false;
        if(gml_id_to_type.find(gml_id) != gml_id_to_type.end()) {
            const auto type = gml_id_to_type.at(gml_id);
            if(type_to_game_mat_id.find(type) != type_to_game_mat_id.end()) {
                ret = true;
                out_next_override_game_mat_id = type_to_game_mat_id.at(type);
            }
        }
        return ret;
    }
}
