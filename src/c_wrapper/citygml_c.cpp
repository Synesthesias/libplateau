#include <exception>
#include <iostream>
#include <citygml/citygml.h>
#include <plateau_dll_logger.h>
#include "libplateau_c.h"
#include "city_model_c.h"

using namespace libplateau;

extern "C" {
    struct plateau_citygml_parser_params {
        bool optimize;
        bool keep_vertices;
        bool tessellate;
        bool ignore_geometries;

        plateau_citygml_parser_params()
            : optimize(true)
            , keep_vertices(true)
            , tessellate(true)
            , ignore_geometries(false) {
        }
    };

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_load_citygml(
            const char* gml_path,
            const plateau_citygml_parser_params params,
            const CityModelHandle** outCityModelPtr,
            const DllLogLevel logLevel,
            LogCallbackFuncPtr logErrorCallback,
            LogCallbackFuncPtr logWarnCallback,
            LogCallbackFuncPtr logInfoCallback) {
        API_TRY{
            citygml::ParserParams parser_params;
            parser_params.optimize = params.optimize;
            parser_params.tesselate = params.tessellate;
            parser_params.keepVertices = params.keep_vertices;
            parser_params.ignoreGeometries = params.ignore_geometries;
            auto logger = std::make_shared<PlateauDllLogger>(logLevel);
            logger->setLogCallbacks(logErrorCallback, logWarnCallback, logInfoCallback);
            logger->log(DllLogLevel::LL_INFO, std::string("Started Parsing gml file.\ngml path = ") + gml_path);
            auto city_model = citygml::load(gml_path, parser_params, logger);
            if (city_model == nullptr) { // 例えば Codelists が見つからない時にエラーになります。
                return APIResult::ErrorLoadingCityGml;
            }
            *outCityModelPtr = new CityModelHandle(city_model);
            logger->log(DllLogLevel::LL_INFO, "Completed parsing gml.");
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
