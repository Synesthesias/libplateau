#include "libplateau_c.h"

#include <plateau/dataset/i_dataset_accessor.h>
#include <filesystem>
#include "citygml/address.h"

using namespace libplateau;
using namespace plateau::dataset;
namespace fs = std::filesystem;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_gml_file(
            GmlFile** gml_file_info_ptr, const char* path
    ) {
        API_TRY {
            const auto gml_file_info = new GmlFile(std::string(path));
            if (gml_file_info->isValid()) {
                *gml_file_info_ptr = gml_file_info;
                return APIResult::Success;
            } else {
                return APIResult::ErrorInvalidArgument;
            }
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_DELETE_FUNC(plateau_delete_gml_file,
                    GmlFile)

    DLL_2_ARG_FUNC(plateau_gml_file_get_path,
                   GmlFile* gml_file,
                   std::string* out_path,
                   *out_path = gml_file->getPath())

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

    DLL_2_ARG_FUNC(plateau_gml_file_search_all_codelist_paths_in_gml,
                   const GmlFile* const gml_file,
                   std::vector<std::string>* codelist_paths,
                   auto set = gml_file->searchAllCodelistPathsInGML();
                   *codelist_paths = std::vector<std::string>(set.cbegin(), set.cend());)

    DLL_2_ARG_FUNC(plateau_gml_file_search_all_image_paths_in_gml,
                   const GmlFile* const gml_file,
                   std::vector<std::string>* image_paths,
                   auto set = gml_file->searchAllImagePathsInGML();
                   *image_paths = std::vector<std::string>(set.cbegin(), set.cend());)

    DLL_VALUE_FUNC(plateau_gml_file_get_max_lod,
                   GmlFile,
                   int,
                   handle->getMaxLod())
}
