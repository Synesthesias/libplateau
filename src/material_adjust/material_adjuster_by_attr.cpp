#include <plateau/material_adjust/material_adjuster_by_attr.h>
#include <plateau/granularity_convert/granularity_converter.h>

namespace plateau::materialAdjust {
    using namespace plateau::granularityConvert;
    using namespace plateau::polygonMesh;


    bool MaterialAdjusterByAttr::registerAttribute(const std::string& gml_id, const std::string& attr) {
        auto result = gml_id_to_attr.insert({gml_id, attr});
        return result.second;
    }

    bool MaterialAdjusterByAttr::registerMaterialPattern(const std::string& attr, const int game_mat_id) {
        auto result = attr_to_game_mat_id.insert({attr, game_mat_id});
        return result.second;
    }

    void MaterialAdjusterByAttr::exec(Model& model) {
        auto materialAdjusterCommon = MaterialAdjusterCommon(this);
        materialAdjusterCommon.exec(model);

    }

    bool MaterialAdjusterByAttr::containsGmlIdKey(const std::string& gml_id) const {
        return gml_id_to_attr.find(gml_id) != gml_id_to_attr.end();
    }

    bool MaterialAdjusterByAttr::containsAttrKey(const std::string& attr) const {
        return attr_to_game_mat_id.find(attr) != attr_to_game_mat_id.end();
    }

    bool MaterialAdjusterByAttr::shouldOverrideMaterial(const std::string& gml_id,
                                                        int& out_next_override_game_mat_id) const {
        bool ret = false;
        if(gml_id_to_attr.find(gml_id) != gml_id_to_attr.end()) {
            const auto attr = gml_id_to_attr.at(gml_id);
            if(attr_to_game_mat_id.find(attr) != attr_to_game_mat_id.end()) {
                ret = true;
                out_next_override_game_mat_id = attr_to_game_mat_id.at(attr);
            }
        }
        return ret;
    }
}
