#pragma once

#include <vector>

#include <plateau/basemap/VectorTileDownloader.h>
#include <plateau/geometry/geo_coordinate.h>

class LIBPLATEAU_EXPORT TileProjection {
public:
<<<<<<< HEAD
    static std::shared_ptr<std::vector<TileCoordinate>> getTileCoordinates(const plateau::geometry::Extent& extent, int zoomLevel);
    static TileCoordinate project(const plateau::geometry::GeoCoordinate& coordinate, int zoom_level);
    static plateau::geometry::Extent unproject(const TileCoordinate& coordinate);
=======
    static std::shared_ptr <std::vector<TileCoordinate>> getTileCoordinates(const plateau::geometry::Extent& extent, int zoomLevel);
    static std::shared_ptr <TileCoordinate> project(const plateau::geometry::GeoCoordinate& coordinate, int zoomLevel);
    static std::shared_ptr <plateau::geometry::GeoCoordinate> unproject(TileCoordinate& coordinate, int zoomLevel);
>>>>>>> d66b8b0fa4c0cda7d16be1403f77b2a569ede64a
};
