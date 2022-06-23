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


    /**
     * \brief 都市モデルからメッシュへの変換設定です。
     */
    struct plateau_mesh_convert_options {
        /**
         * \brief 出力後のメッシュの座標系を指定します。
         */
        AxesConversion mesh_axes;

        /**
         * \brief 変換時の基準点を指定します。
         *
         * 基準点は平面直角座標であり、メッシュは基準点からの相対座標で出力されます。
         */
        plateau_vector3d reference_point;

        /**
         * \brief 出力後のメッシュの粒度(結合単位)を指定します。
         */
        MeshGranularity mesh_granularity;

        /**
         * \brief 出力後のメッシュに含める最小のLODを指定します。
         */
        unsigned min_lod;

        /**
         * \brief 出力後のメッシュに含める最大のLODを指定します。
         */
        unsigned max_lod;

        /**
         * \brief 1つの地物について複数のLODがある場合に最大LOD以外のジオメトリを出力するかどうかを指定します。
         */
        bool export_lower_lod;

        /**
         * \brief テクスチャ、マテリアル情報を出力するかどうかを指定します。
         */
        bool export_appearance;

        /**
         * \brief 緯度経度からメートルへの変換を行うかどうかを指定します。
         */
        bool convert_lat_lon;

        static MeshConvertOptions convert_to(const plateau_mesh_convert_options& options) {
            MeshConvertOptions result;
            result.mesh_axes = options.mesh_axes;
            result.reference_point = plateau_vector3d::convert_to(options.reference_point);
            result.mesh_granularity = options.mesh_granularity;
            result.min_lod = options.min_lod;
            result.max_lod = options.max_lod;
            result.export_lower_lod = options.export_lower_lod;
            result.export_appearance = options.export_appearance;
            result.convert_lat_lon = options.convert_lat_lon;
            return result;
        }

        static plateau_mesh_convert_options convert_from(const MeshConvertOptions& options) {
            plateau_mesh_convert_options result;
            result.mesh_axes = options.mesh_axes;
            result.reference_point = plateau_vector3d::convert_from(options.reference_point);
            result.mesh_granularity = options.mesh_granularity;
            result.min_lod = options.min_lod;
            result.max_lod = options.max_lod;
            result.export_lower_lod = options.export_lower_lod;
            result.export_appearance = options.export_appearance;
            result.convert_lat_lon = options.convert_lat_lon;
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
                 plateau_mesh_convert_options,
                 plateau_mesh_convert_options::convert_from(handle->getOptions()))

        LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_converter_set_options(
                MeshConverter* mesh_converter, const plateau_mesh_convert_options options) {
        API_TRY{
            mesh_converter->setOptions(plateau_mesh_convert_options::convert_to(options));
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT plateau_mesh_convert_options LIBPLATEAU_C_API plateau_create_mesh_convert_options() {
        API_TRY{
            const MeshConvertOptions options;
            return plateau_mesh_convert_options::convert_from(options);
        }
        API_CATCH;
        return {};
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_convert_options_set_valid_reference_point(
            plateau_mesh_convert_options* options, CityModelHandle* city_model_handle) {
        API_TRY{
            auto copied_options = plateau_mesh_convert_options::convert_to(*options);
            MeshConvertOptionsFactory::setValidReferencePoint(copied_options, city_model_handle->getCityModel());
            *options = plateau_mesh_convert_options::convert_from(copied_options);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
