#include "libplateau_c.h"

#include <plateau/udx/local_dataset_accessor.h>

#include "citygml/address.h"

using namespace libplateau;
using namespace plateau::udx;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_gml_file(
            GmlFile** gml_file_info_ptr, const char* path
    ) {
        API_TRY {
            const auto gml_file_info = new GmlFile(path);
            if (gml_file_info->isValid()) {
                *gml_file_info_ptr = gml_file_info;
                return APIResult::Success;
            } else {
                return APIResult::ErrorInvalidArgument;
            }
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_gml_file(
            const GmlFile* gml_file_info
    ) {
        delete gml_file_info;
        return APIResult::Success;
    }

    DLL_STRING_PTR_FUNC(plateau_gml_file_get_path,
                        GmlFile,
                        handle->getPath())

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_gml_file_set_path(
            GmlFile* gml_file_info,
            char* path
    ) {
        API_TRY {
            gml_file_info->setPath(std::string(path));
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_STRING_PTR_FUNC(plateau_gml_file_get_feature_type_str,
                        GmlFile,
                        handle->getFeatureType())

    DLL_VALUE_FUNC(plateau_gml_file_get_mesh_code,
                   GmlFile,
                   MeshCode,
                   handle->getMeshCode())

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_gml_file_fetch(
            const GmlFile* const gml_file_info,
            char* destination_root_path_chars,
            GmlFile* const out_gml_file_info
    ) {
        API_TRY {
            const auto destination_root_path = std::string(destination_root_path_chars);
            gml_file_info->fetch(destination_root_path, *out_gml_file_info);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

}
