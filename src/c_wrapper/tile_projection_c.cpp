#include "libplateau_c.h"
#include <plateau/basemap/TileProjection.h>
extern "C" {
    using namespace libplateau;
    using namespace plateau::geometry;

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_tile_projection_project(
            GeoCoordinate geo_coordinate,
            int zoom_level,
            TileCoordinate* const out_tile_coordinate
            ){
        API_TRY{
            auto tile_coord = TileProjection::project(geo_coordinate, zoom_level);
            *out_tile_coordinate = tile_coord;
            return APIResult::Success;
        }API_CATCH
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_tile_projection_unproject(
            TileCoordinate tile_coordinate,
            Extent* const out_extent
            ){
        API_TRY{
            auto extent = TileProjection::unproject(tile_coordinate);
            *out_extent = extent;
            return APIResult::Success;
        }API_CATCH
        return APIResult::ErrorUnknown;
    }

}
