#include <plateau/material_adjust/material_adjuster_by_type.h.>
#include "libplateau_c.h"
using namespace plateau::materialAdjust;
using namespace libplateau;
using namespace citygml;
extern "C" {
    DLL_CREATE_FUNC(create_material_adjuster_by_type, MaterialAdjusterByType)

    DLL_DELETE_FUNC(delete_material_adjuster_by_type, MaterialAdjusterByType)

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API material_adjuster_by_type_register_type(
        MaterialAdjusterByType* const adjuster,
        const char* const gml_id_char,
        const CityObject::CityObjectsType type,
        bool* const out_succeed
    ) {
        API_TRY {
            const auto gml_id_str = std::string(gml_id_char);
            *out_succeed = adjuster->registerType(std::string(gml_id_str), type);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API material_adjuster_by_type_register_material_pattern(
        MaterialAdjusterByType* const adjuster,
        const CityObject::CityObjectsType type,
        const int game_mat_id,
        bool* const out_succeed
    ) {
        API_TRY {
            *out_succeed = adjuster->registerMaterialPattern(type, game_mat_id);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_2_ARG_FUNC(material_adjuster_by_type_exec, MaterialAdjusterByType* const adjuster, Model* const model,
                    adjuster->exec(*model))
}
