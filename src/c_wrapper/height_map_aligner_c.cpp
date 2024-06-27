#include "libplateau_c.h"
#include <plateau/height_map_alighner/height_map_aligner.h>
using namespace plateau::heightMapAligner;
using namespace plateau::polygonMesh;
using namespace libplateau;

extern "C" {

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API height_map_aligner_create(
            HeightMapAligner** aligner,
            double height_offset
    ) {
        API_TRY{
            *aligner = new HeightMapAligner(height_offset);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API height_map_aligner_destroy(
            HeightMapAligner* aligner
    ) {
        API_TRY{
            delete aligner;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API height_map_aligner_add_heightmap_frame(
            HeightMapAligner* const aligner,
            const uint16_t* heightmap,
            int heightmap_size, // size = width * height
            int heightmap_width,
            int heightmap_height,
            float min_x,
            float max_x,
            float min_y,
            float max_y,
            float min_height,
            float max_height
    ) {
        API_TRY{
            if(heightmap_size != heightmap_width * heightmap_height) {
                return APIResult::ErrorInvalidArgument;
            }

            auto map = HeightMapFrame(std::vector<uint16_t>(heightmap, heightmap + heightmap_size), heightmap_width,
                                      heightmap_height, min_x, max_x, max_y, min_y, min_height,
                                      max_height);
            aligner->addHeightmapFrame(map);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API height_map_aligner_align(
            const HeightMapAligner* const aligner,
            Model* const model
    ) {
        API_TRY{
            if(aligner->heightmapCount() == 0) {
                return APIResult::NotPreparedForOperation;
            }
            aligner->align(*model);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
