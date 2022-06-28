#include <exception>
#include <iostream>

#include <plateau/io/mesh_converter.h>

#include "libplateau_c.h"
#include "city_model_c.h"
#include "plateau/io/mesh_convert_options_factory.h"

using namespace libplateau;

extern "C" {
    struct plateau_vector3d {
        double x;
        double y;
        double z;

        static TVec3d convert_to(const plateau_vector3d& vec) {
            TVec3d result;
            result.x = vec.x;
            result.y = vec.y;
            result.z = vec.z;
            return result;
        }

        static plateau_vector3d convert_from(const TVec3d& vec) {
            plateau_vector3d result;
            result.x = vec.x;
            result.y = vec.y;
            result.z = vec.z;
            return result;
        }
    };

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_mesh_converter(MeshConverter** mesh_converter) {
        API_TRY{
            *mesh_converter = new MeshConverter;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_delete_mesh_converter(MeshConverter* mesh_converter) {
        API_TRY{
            delete mesh_converter;
        }
        API_CATCH;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_converter_convert(
            const MeshConverter* mesh_converter,
            const char* destination_directory,
            const char* gml_path, const CityModelHandle* city_model,
            PlateauDllLogger* logger) {
        API_TRY{
            const std::shared_ptr<PlateauDllLogger> logger_ptr(logger, [](auto) {});
            if (city_model == nullptr) {
                mesh_converter->convert(destination_directory, gml_path, nullptr, logger_ptr);
            } else {
                mesh_converter->convert(destination_directory, gml_path, city_model->getCityModelPtr(), logger_ptr);
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    DLL_VALUE_FUNC(plateau_mesh_converter_get_options,
                 MeshConverter,
                 MeshConvertOptions,
                 handle->getOptions())

        LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_converter_set_options(
                MeshConverter* mesh_converter, const MeshConvertOptions options) {
        API_TRY{
            mesh_converter->setOptions(options);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT MeshConvertOptions LIBPLATEAU_C_API plateau_create_mesh_convert_options() {
        API_TRY{
            const MeshConvertOptions options;
            return options;
        }
        API_CATCH;
        return {};
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_convert_options_set_valid_reference_point(
            MeshConvertOptions* options, CityModelHandle* city_model_handle) {
        API_TRY{
            MeshConvertOptionsFactory::setValidReferencePoint(*options, city_model_handle->getCityModel());
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
