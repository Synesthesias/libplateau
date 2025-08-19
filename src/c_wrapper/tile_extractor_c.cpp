#include "libplateau_c.h"
#include "city_model_c.h"
#include <plateau/polygon_mesh/tile_extractor.h>
#include <plateau/geometry/geo_coordinate.h>

using namespace libplateau;
using namespace plateau::polygonMesh;

extern "C"{

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_tile_extractor_extract_with_combine(
        const CityModelHandle** const city_model_handles,
        const int city_model_size,
        const MeshExtractOptions options,
        const std::vector<plateau::geometry::Extent>* extents,
        Model* const out_model) {
        API_TRY{

            CityModelVector city_models = std::make_shared<std::vector<std::weak_ptr<const citygml::CityModel>>>();
            for (int i = 0; i < city_model_size; ++i) {
                if (!city_model_handles[i]) {
                    continue; // nullptr の場合はスキップ  
                }
                const auto & ptr = city_model_handles[i]->getCityModelPtr(); // ここで city_model_handles[i] のポインタを取得
                std::weak_ptr<const citygml::CityModel> weak = ptr;
                city_models->push_back(weak);
            }

            TileExtractor::extractWithCombine(*out_model, city_models, options, *extents);
            return APIResult::Success;

        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_tile_extractor_extract_with_grid(
        const CityModelHandle* const city_model_handle,
        const MeshExtractOptions options,
        const std::vector<plateau::geometry::Extent>* extents,
        Model* const out_model) {
        API_TRY{
            TileExtractor::extractWithGrid(*out_model, city_model_handle->getCityModel(), options, *extents);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
