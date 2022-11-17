#include "libplateau_c.h"

#include <plateau/udx/local_dataset_accessor.h>
#include <filesystem>

using namespace libplateau;
using namespace plateau::udx;
using namespace plateau::geometry;
namespace fs = std::filesystem;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_local_dataset_accessor(
            LocalDatasetAccessor** out_collection_ptr
    ) {
        *out_collection_ptr = new LocalDatasetAccessor();
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_local_dataset_accessor(
            const LocalDatasetAccessor* collection_ptr
    ) {
        delete collection_ptr;
        return APIResult::Success;
    }

    DLL_VALUE_FUNC(plateau_local_dataset_accessor_get_mesh_code_count,
                   LocalDatasetAccessor,
                   int,
                   static_cast<int>(handle->getMeshCodes().size()))

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_local_dataset_accessor_get_mesh_codes(
            LocalDatasetAccessor* handle, MeshCode* mesh_codes, int count) {
        API_TRY {
            const auto& result = handle->getMeshCodes();
            int i = 0;
            for (const auto& mesh_code: result) {
                mesh_codes[i] = mesh_code;
                ++i;
                if (i >= count) break;
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_local_dataset_accessor_find(
            const char* source_char, LocalDatasetAccessor* out_collection) {
        try {
            auto source_str = std::string(source_char);
            LocalDatasetAccessor::find(source_str, *out_collection);
            return APIResult::Success;
        } catch (fs::filesystem_error& e) {
            return APIResult::ErrorFileSystem;
        } catch (...) {
            return APIResult::ErrorUnknown;
        }
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_local_dataset_accessor_filter(
            LocalDatasetAccessor* handle, plateau::geometry::Extent extent, LocalDatasetAccessor* out_collection) {
        API_TRY {
            handle->filter(extent, *out_collection);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_local_dataset_accessor_filter_by_mesh_codes(
            const LocalDatasetAccessor* const handle,
            const MeshCode* const mesh_code_array,
            int mesh_codes_count,
            LocalDatasetAccessor* const out_collection
    ) {
        API_TRY {
            auto mesh_codes = std::vector<MeshCode>();
            for (int i = 0; i < mesh_codes_count; i++) {
                mesh_codes.push_back(mesh_code_array[i]);
            }
            handle->filterByMeshCodes(mesh_codes, *out_collection);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_VALUE_FUNC(plateau_local_dataset_accessor_get_packages,
                   LocalDatasetAccessor,
                   PredefinedCityModelPackage,
                   handle->getPackages())

    DLL_VALUE_FUNC(plateau_local_dataset_accessor_get_gml_file_count,
                   LocalDatasetAccessor,
                   int,
                   handle->getGmlFileCount(package), , PredefinedCityModelPackage package)

    DLL_STRING_PTR_FUNC(plateau_local_dataset_accessor_get_gml_file,
                        LocalDatasetAccessor,
                        handle->getGmlFilePath(package, index), , PredefinedCityModelPackage package, int index)

    DLL_VALUE_FUNC(plateau_local_dataset_accessor_center_point,
                   LocalDatasetAccessor,
                   TVec3d,
                   handle->calculateCenterPoint(*geo_reference), , GeoReference* geo_reference)

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_udx_sub_folder_get_package(
            const char* const dir_name, PredefinedCityModelPackage* const out_package) {
        API_TRY {
            *out_package = UdxSubFolder::getPackage(dir_name);
            return APIResult::Success;
        } API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
