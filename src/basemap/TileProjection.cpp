#include <plateau/basemap/TileProjection.h>
#define _USE_MATH_DEFINES
#include <math.h>

namespace {
    int long2tileX(double longitude, int z) {
<<<<<<< HEAD
        return static_cast<int>(floor((longitude + 180.0) / 360.0 * (1 << z)));
    }

    int lat2tileY(double latitude, int z) {
        const double latrad = latitude * M_PI / 180.0;
        return static_cast<int>(floor((1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << z)));
    }

    double tileX2long(int x, int z) {
        return x / static_cast<double>(1 << z) * 360.0 - 180;
    }

    double tileY2lat(int y, int z) {
        const double n = M_PI - 2.0 * M_PI * y / static_cast<double>(1 << z);
=======
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
>>>>>>> d66b8b0fa4c0cda7d16be1403f77b2a569ede64a
        return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
    }
}

<<<<<<< HEAD
std::shared_ptr<std::vector<TileCoordinate>> TileProjection::getTileCoordinates(const plateau::geometry::Extent& extent, int zoomLevel) {
    const int min_x = long2tileX(extent.min.longitude, zoomLevel);
    const int min_y = lat2tileY(extent.max.latitude, zoomLevel);
    const int max_x = long2tileX(extent.max.longitude, zoomLevel);
    const int max_y = lat2tileY(extent.min.latitude, zoomLevel);

    auto tileCoordinates = std::make_shared<std::vector<TileCoordinate>>();
    for (int x = min_x; x <= max_x; x++) {
        for (int y = min_y; y <= max_y; y++) {
            TileCoordinate coordinate{};
=======
std::shared_ptr <std::vector<TileCoordinate>> TileProjection::getTileCoordinates(const plateau::geometry::Extent& extent, int zoomLevel) {
    int minX = long2tileX(extent.min.longitude, zoomLevel);
    int minY = lat2tileY(extent.max.latitude, zoomLevel);
    int maxX = long2tileX(extent.max.longitude, zoomLevel);
    int maxY = lat2tileY(extent.min.latitude, zoomLevel);

    auto tileCoordinates = std::make_shared<std::vector<TileCoordinate>>();
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            TileCoordinate coordinate;
>>>>>>> d66b8b0fa4c0cda7d16be1403f77b2a569ede64a
            coordinate.column = x;
            coordinate.row = y;
            coordinate.zoom_level = zoomLevel;
            tileCoordinates->push_back(coordinate);
        }
    }

    return tileCoordinates;
}

<<<<<<< HEAD
TileCoordinate TileProjection::project(const plateau::geometry::GeoCoordinate& coordinate, const int zoom_level) {
    TileCoordinate tile_coordinate;
    tile_coordinate.column = long2tileX(coordinate.longitude, zoom_level);
    tile_coordinate.row = lat2tileY(coordinate.latitude, zoom_level);
    tile_coordinate.zoom_level = zoom_level;

    return tile_coordinate;
}

plateau::geometry::Extent TileProjection::unproject(const TileCoordinate& coordinate) {
    return {
        {
            tileY2lat(coordinate.row + 1, coordinate.zoom_level),
            tileX2long(coordinate.column, coordinate.zoom_level),
            0
        },
        {
            tileY2lat(coordinate.row, coordinate.zoom_level),
            tileX2long(coordinate.column + 1, coordinate.zoom_level),
            0
        }
    };
=======
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
>>>>>>> d66b8b0fa4c0cda7d16be1403f77b2a569ede64a
}
