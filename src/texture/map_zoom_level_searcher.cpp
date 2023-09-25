#include <plateau/texture/map_zoom_level_searcher.h>
#include <plateau/basemap/tile_projection.h>
#include <plateau/basemap/vector_tile_downloader.h>
#include <httplib.h>


namespace plateau::texture {
    MapZoomLevelSearchResult MapZoomLevelSearcher::search(const std::string& url_template, const geometry::GeoCoordinate geo_coord) {
        constexpr int zoom_level_search_range_min = 1;
        constexpr int zoom_level_search_range_max = 18;
        int found_min = 9999;
        int found_max = -1;
        bool is_succeed = false;
        for(int zoom = zoom_level_search_range_min; zoom <= zoom_level_search_range_max; zoom++) {
            auto tile_coord = TileProjection::project(geo_coord, zoom);
            std::string body;
            const auto result = VectorTileDownloader::httpRequest(url_template, tile_coord, body);

            // サーバーに接続できないときは即時失敗とします。
            if(result.error() != httplib::Error::Success) {
                is_succeed = false;
                break;
            }
            // サーバーに接続できるが、ズームレベルが未対応のとき、通常200以外のステータスコードが返ります。
            if(result->status == 200) {
                found_min = std::min(zoom, found_min);
                found_max = std::max(zoom, found_max);
                is_succeed = true;
            }
        }
        return {is_succeed, found_min, found_max};

    }
}
