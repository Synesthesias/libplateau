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

    std::string plateau_vector_downloader_c_last_downloaded_image_path;


    /**
     * image_path を取得するためには、このメソッドを呼んだ後に
     * plateau_vector_tile_downloader_download_last_image_path を呼んでください。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_vector_tile_downloader_download(
            const VectorTileDownloader* const downloader,
            int index,
            TileCoordinate* const tile_coordinate,
            int* size_of_image_path
    ) {
        API_TRY {
            auto tile = VectorTile();
            if (index >= downloader->getTileCount()) {
                return APIResult::ErrorIndexOutOfBounds;
            }
            downloader->download(index, tile);
            *tile_coordinate = tile.coordinate;
            const auto& image_path = tile.image_path;
            plateau_vector_downloader_c_last_downloaded_image_path = image_path;
            *size_of_image_path = (int) image_path.size();
            return APIResult::Success;
        } API_CATCH
        return APIResult::ErrorUnknown;
    }

    /**
     * plateau_vector_tile_downloader_download のあとに呼ぶべきメソッドで、download の結果である image_path を取得します。
     * C++ のstringを C#側にコピーするためには、文字列長を取得 → 文字列をコピー の2段構成となります。
     * このメソッドは2段構成の2段目の部分です。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_vector_tile_downloader_last_image_path(
            char* const out_url_str_ptr
    ) {
        API_TRY {
            const auto& str = plateau_vector_downloader_c_last_downloaded_image_path;
            auto chars = str.c_str();
            auto len = (dll_str_size_t) (str.length());
            strncpy(out_url_str_ptr, chars, len);
            out_url_str_ptr[len] = '\0'; // null終端
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
