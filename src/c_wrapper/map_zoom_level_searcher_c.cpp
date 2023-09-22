#include "libplateau_c.h"
#include <plateau/texture/map_zoom_level_searcher.h>
using namespace plateau::texture;
using namespace plateau::geometry;
using namespace libplateau;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_map_zoom_level_searcher_search(
            MapZoomLevelSearchResult* const out_result,
            char* url_template,
            double latitude, double longitude
            ) {
        API_TRY {
            const auto coord = GeoCoordinate(latitude, longitude, 0);
            *out_result = MapZoomLevelSearcher::search(std::string(url_template), coord);
            return libplateau::APIResult::Success;
        }API_CATCH
        return libplateau::APIResult::ErrorUnknown;
    }
}
