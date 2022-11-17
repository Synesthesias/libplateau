#include "libplateau_c.h"
#include <plateau/basemap/vector_tile_downloader.h>

extern "C" {
using namespace libplateau;
using namespace plateau::geometry;

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_vector_tile_downloader(
            VectorTileDownloader** out_vector_tile_downloader,
            const char* destination,
            Extent extent,
            int zoom_level
    ) {
        *out_vector_tile_downloader = new VectorTileDownloader(destination, extent, zoom_level);
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_vector_tile_downloader(
            const VectorTileDownloader* vector_tile_downloader
    ) {
        delete vector_tile_downloader;
        return APIResult::Success;
    }

    DLL_VALUE_FUNC(plateau_vector_tile_downloader_get_tile_count,
                   VectorTileDownloader,
                   int,
                   handle->getTileCount())

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_vector_tile_downloader_download(
            const VectorTileDownloader* const downloader,
            int index
    ) {
        API_TRY {
            auto tile = VectorTile();
            if (index >= downloader->getTileCount()) {
                return APIResult::ErrorIndexOutOfBounds;
            }
            auto result = downloader->download(index, tile);
            if(!result) return APIResult::ErrorValueNotFound;
            return APIResult::Success;
        } API_CATCH
        return APIResult::ErrorUnknown;
    }

    DLL_STRING_VALUE_FUNC(plateau_vector_tile_downloader_calc_destination_path,
                          VectorTileDownloader,
                          handle->calcDestinationPath(index).string(), , int index)

    DLL_VALUE_FUNC(plateau_vector_tile_downloader_get_tile,
                   VectorTileDownloader,
                   TileCoordinate,
                   handle->getTile(index), , int index)
}
