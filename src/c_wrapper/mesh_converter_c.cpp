#include <exception>
#include <iostream>

#include <plateau/io/mesh_converter.h>

#include "libplateau_c.h"
#include "city_model_c.h"

using namespace libplateau;

extern "C" {
    struct plateau_vector3d {
        double x;
        double y;
        double z;
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

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_converter_convert(MeshConverter* mesh_converter, const char* destination_directory, const char* gml_path, const CityModelHandle* city_model) {
        API_TRY{
            if (city_model == nullptr) {
                mesh_converter->convert(destination_directory, gml_path);
            } else {
                mesh_converter->convert(destination_directory, gml_path, city_model->getCityModelPtr());
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

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_converter_set_options(MeshConverter* mesh_converter, MeshConvertOptions options) {
        API_TRY{
            mesh_converter->setOptions(options);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
