#include <plateau/material_adjust/material_adjuster_by_attr.h>
#include "libplateau_c.h"
#include "plateau/polygon_mesh/model.h"

using namespace plateau::materialAdjust;
using namespace plateau::polygonMesh;
using namespace libplateau;

extern "C" {
    DLL_CREATE_FUNC(create_material_adjuster_by_attr, MaterialAdjusterByAttr)

    DLL_DELETE_FUNC(delete_material_adjuster_by_attr, MaterialAdjusterByAttr)

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API material_adjuster_by_attr_register_attribute(
        MaterialAdjusterByAttr* const adjuster,
        const char* const gml_id_char,
        const char* const attr,
        bool* const out_succeed
    ) {
        API_TRY {
            const auto gml_id_str = std::string(gml_id_char);
            *out_succeed = adjuster->registerAttribute(std::string(gml_id_str), std::string(attr));
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API material_adjuster_by_attr_register_material_pattern(
        MaterialAdjusterByAttr* const adjuster,
        const char* const attr_char,
        const int game_mat_id,
        bool* const out_succeed
    ) {
        API_TRY {
            const auto attr_str = std::string(attr_char);
            *out_succeed = adjuster->registerMaterialPattern(attr_str, game_mat_id);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_2_ARG_FUNC(material_adjuster_by_attr_exec, MaterialAdjusterByAttr* const adjuster, Model* const model,
                    adjuster->exec(*model))
}
