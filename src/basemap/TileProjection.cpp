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

    double tileX2long(int x, int z) {
        return x / (double)(1 << z) * 360.0 - 180;
    }

    double tileY2lat(int y, int z) {
        double n = M_PI - 2.0 * M_PI * y / (double)(1 << z);
        return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
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

std::shared_ptr <TileCoordinate> TileProjection::project(const plateau::geometry::GeoCoordinate& coordinate, int zoomLevel) {
    auto tileCoordinate = std::make_shared<TileCoordinate>();
    tileCoordinate->column = long2tileX(coordinate.longitude, zoomLevel);
    tileCoordinate->row = lat2tileY(coordinate.latitude, zoomLevel);
    tileCoordinate->zoom_level = zoomLevel;

    return tileCoordinate;
}

std::shared_ptr<plateau::geometry::GeoCoordinate> TileProjection::unproject(TileCoordinate& coordinate, int zoomLevel) {
    auto geoCoordinate = std::make_shared<plateau::geometry::GeoCoordinate>();
    geoCoordinate->latitude = tileY2lat(coordinate.row, zoomLevel);
    geoCoordinate->longitude = tileX2long(coordinate.column, zoomLevel);

    return geoCoordinate;
}
