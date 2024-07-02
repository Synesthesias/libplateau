#include "libplateau_c.h"
#include <plateau/height_map_alighner/height_map_aligner.h>
#include <plateau/geometry/geo_coordinate.h>
#include <cstring>
using namespace plateau::heightMapAligner;
using namespace plateau::heightMapGenerator;
using namespace plateau::polygonMesh;
using namespace plateau::geometry;
using namespace libplateau;

extern "C" {

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API height_map_aligner_create(
            HeightMapAligner** aligner,
            double height_offset,
            CoordinateSystem axis
    ) {
        API_TRY{
            *aligner = new HeightMapAligner(height_offset, axis);
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
            const HeightMapElemT* heightmap,
            const int heightmap_size, // size = width * height
            const int heightmap_width,
            const int heightmap_height,
            const float min_x,
            const float max_x,
            const float min_y,
            const float max_y,
            const float min_height,
            const float max_height,
            const CoordinateSystem axis
    ) {
        API_TRY{
            if(heightmap_size != heightmap_width * heightmap_height) {
                return APIResult::ErrorInvalidArgument;
            }

            auto map = HeightMapFrame(HeightMapT(heightmap, heightmap + heightmap_size), heightmap_width,
                                      heightmap_height, min_x, max_x, max_y, min_y, min_height,
                                      max_height, axis);
            aligner->addHeightmapFrame(map);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API height_map_aligner_align(
            HeightMapAligner* const aligner,
            Model* const model,
            const float max_edge_length
    ) {
        API_TRY{
            if(aligner->heightmapCount() == 0) {
                return APIResult::NotPreparedForOperation;
            }
            aligner->align(*model, max_edge_length);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API height_map_aligner_align_invert(
            HeightMapAligner* const aligner,
            Model* const model,
            const int alpha_expand_width_cartesian,
            const int alpha_average_width_cartesian,
            const float height_offset,
            const float skip_threshold_of_map_land_distance
    ) {
        API_TRY{
            if(aligner->heightmapCount() == 0) {
                return APIResult::NotPreparedForOperation;
            }
            aligner->alignInvert(*model, alpha_expand_width_cartesian, alpha_average_width_cartesian, height_offset, skip_threshold_of_map_land_distance);
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API height_map_aligner_height_map_count(
            HeightMapAligner* const aligner,
            int* out_height_map_count
    ) {
        API_TRY{
            *out_height_map_count = aligner->heightmapCount();
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API height_map_aligner_get_height_map_at(
            HeightMapAligner* const aligner,
            int index,
            HeightMapElemT** out_height_map,
            int* data_size
    ) {
        API_TRY{
            auto height_map =  aligner->getHeightMapFrameAt(index).heightmap;
            auto copied_map = new HeightMapElemT[height_map.size()];
            memcpy(copied_map, height_map.data(), sizeof(HeightMapElemT) * height_map.size());
            *out_height_map = copied_map;
            *data_size = (int)height_map.size();
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}
