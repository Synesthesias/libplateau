#include <plateau/basemap/TileProjection.h>
#define _USE_MATH_DEFINES
#include <math.h>

namespace {
    int long2tileX(double longitude, int z) {
        return (int)(floor((longitude + 180.0) / 360.0 * (1 << z)));
    }

    int lat2tileY(double latitude, int z) {
        double latrad = latitude * M_PI / 180.0;
        return (int)(floor((1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << z)));
    }
}

std::shared_ptr <std::vector<TileCoordinate>> TileProjection::getTileCoordinates(const plateau::geometry::Extent& extent, int zoomLevel) {
    int minX = long2tileX(extent.min.longitude, zoomLevel);
    int minY = lat2tileY(extent.max.latitude, zoomLevel);
    int maxX = long2tileX(extent.max.longitude, zoomLevel);
    int maxY = lat2tileY(extent.min.latitude, zoomLevel);

    auto tileCoordinates = std::make_shared<std::vector<TileCoordinate>>();
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            TileCoordinate coordinate;
            coordinate.column = x;
            coordinate.row = y;
            coordinate.zoom_level = zoomLevel;
            tileCoordinates->push_back(coordinate);
        }
    }

    return tileCoordinates;
}
