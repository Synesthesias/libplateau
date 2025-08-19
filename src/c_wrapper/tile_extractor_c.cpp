#include "libplateau_c.h"
#include "city_model_c.h"
#include <plateau/polygon_mesh/tile_extractor.h>

using namespace libplateau;
using namespace plateau::polygonMesh;

extern "C"{

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_tile_extractor_extract_with_combine(
        const CityModelHandle* const* city_model_handles,
        const int city_model_size,
        const MeshExtractOptions options,
        const std::vector<plateau::geometry::Extent>* extents,
        Model* const out_model) {
        API_TRY{

            if (out_model == nullptr || extents == nullptr) {
                return APIResult::ErrorInvalidArgument;
            }
            if (city_model_size < 0) {
                return APIResult::ErrorInvalidArgument;
            }
                if (city_model_size > 0 && city_model_handles == nullptr) {
                return APIResult::ErrorInvalidArgument;
            }

            CityModelVector city_models = std::make_shared<std::vector<std::weak_ptr<const citygml::CityModel>>>();
            city_models->reserve(static_cast<size_t>(city_model_size));
            for (int i = 0; i < city_model_size; ++i) {
                if (!city_model_handles[i]) {
                    continue; // nullptr の場合はスキップ  
                }
                auto ptr = city_model_handles[i]->getCityModelPtr(); // 共有所有権を値で受ける
                if (!ptr) {
                    continue; // 空の shared_ptr はスキップ 
                }
                std::weak_ptr<const citygml::CityModel> weak = ptr;
                city_models->push_back(weak);
            }

            if (city_models->empty()) {
                // 入力に有効な CityModel が含まれていない
                return APIResult::ErrorInvalidArgument;   
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
            if (out_model == nullptr || extents == nullptr || city_model_handle == nullptr) {
                return APIResult::ErrorInvalidArgument;
            }
            TileExtractor::extractWithGrid(*out_model, city_model_handle->getCityModel(), options, *extents);
            auto cityModelPtr = city_model_handle->getCityModelPtr();
            if (!cityModelPtr) {
                return APIResult::ErrorInvalidArgument;
            }
            TileExtractor::extractWithGrid(*out_model, *cityModelPtr, options, *extents);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
