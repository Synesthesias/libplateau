#include "libplateau_c.h"

#include <plateau/udx/udx_file_collection.h>

using namespace libplateau;
using namespace plateau::udx;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_udx_file_collection(
        UdxFileCollection** out_collection_ptr
    ) {
        *out_collection_ptr = new UdxFileCollection();
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_udx_file_collection(
        const UdxFileCollection* collection_ptr
    ) {
        delete collection_ptr;
        return APIResult::Success;
    }

    DLL_VALUE_FUNC(plateau_udx_file_collection_get_mesh_code_count,
                   UdxFileCollection,
                   int,
                   static_cast<int>(handle->getMeshCodes().size()))

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_udx_file_collection_get_mesh_codes(
        UdxFileCollection* handle, MeshCode* mesh_codes, int count) {
        API_TRY{
            const auto& result = handle->getMeshCodes();
            int i=0;
            for (const auto& mesh_code : result){
                mesh_codes[i] = mesh_code;
                ++i;
                if (i >= count) break;
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_udx_file_collection_find(
        const char* source, UdxFileCollection* out_collection) {
        API_TRY{
            UdxFileCollection::find(source, *out_collection);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_udx_file_collection_filter(
        UdxFileCollection* handle, plateau::geometry::Extent extent, UdxFileCollection* out_collection) {
        API_TRY{
            handle->filter(extent, *out_collection);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_udx_file_collection_filter_by_mesh_codes(
            const UdxFileCollection* const handle,
            const MeshCode* const mesh_code_array,
            int mesh_codes_count,
            UdxFileCollection* const out_collection
            ){
        API_TRY{
            auto mesh_codes = std::vector<MeshCode>();
            for(int i=0; i<mesh_codes_count; i++){
                mesh_codes.push_back(mesh_code_array[i]);
                handle->filter_by_mesh_codes(mesh_codes, *out_collection);
            }
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_udx_file_collection_fetch(
            UdxFileCollection* handle,
            char* destination_root_path_chars,
            const GmlFileInfo* const gml_file_info
            ){
        API_TRY{
            auto destination_root_path = std::string(destination_root_path_chars);
            handle->fetch(destination_root_path, *gml_file_info);
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_VALUE_FUNC(plateau_udx_file_collection_get_packages,
               UdxFileCollection,
               PredefinedCityModelPackage,
               handle->getPackages())

    DLL_VALUE_FUNC(plateau_udx_file_collection_get_gml_file_count,
               UdxFileCollection,
               int,
               handle->getGmlFileCount(package),
               ,PredefinedCityModelPackage package)

    DLL_STRING_PTR_FUNC(plateau_udx_file_collection_get_gml_file,
                        UdxFileCollection,
                        handle->getGmlFilePath(package, index),
                        ,PredefinedCityModelPackage package
                        ,int index)

    //LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_udx_file_collection_get_gml_file(
    //    UdxFileCollection* handle, PredefinedCityModelPackage package, int index, char* path, int* count) {
    //    API_TRY{
    //        std::vector<GmlFileInfo> result;
    //        handle->getGmlFiles(package, result);
    //        for (size_t i = 0; i < result.size(); ++i) {
    //            files[i] = result[i];
    //        }
    //        return APIResult::Success;
    //    }
    //    API_CATCH;
    //    return APIResult::ErrorUnknown;
    //}
}
