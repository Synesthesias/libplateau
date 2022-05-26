#include <exception>
#include <iostream>

#include <citygml/citygml.h>

#include "libplateau_c.h"
#include "city_model_c.h"

using namespace libplateau;

extern "C" {
    struct plateau_citygml_parser_params {
        bool optimize;
        bool tessellate;

        plateau_citygml_parser_params()
            : optimize(true),
              tessellate(true){
        }
    };

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_load_citygml(
            const char* gml_path,
            const plateau_citygml_parser_params params,
            const CityModelHandle** outCityModelPtr) {
        API_TRY{
            citygml::ParserParams parser_params;
            parser_params.optimize = params.optimize;
            parser_params.tesselate = params.tessellate;
            auto city_model = citygml::load(gml_path, parser_params, nullptr);
            if(city_model == nullptr){ // 例えば Codelists が見つからない時にエラーになります。
                return APIResult::ErrorLoadingCityGml;
            }
            *outCityModelPtr = new CityModelHandle(city_model);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}