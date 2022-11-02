#pragma once

#include <memory>
#include <libplateau_api.h>
#include "plateau/geometry/geo_coordinate.h"


struct TileCoordinate {
    int column;
<<<<<<< HEAD
    int row;
=======

    int row;

>>>>>>> d66b8b0fa4c0cda7d16be1403f77b2a569ede64a
    int zoom_level;
};

struct VectorTile {
<<<<<<< HEAD
    TileCoordinate coordinate{};
=======
    TileCoordinate coordinate;

>>>>>>> d66b8b0fa4c0cda7d16be1403f77b2a569ede64a
    std::string image_path;
};

class LIBPLATEAU_EXPORT VectorTileDownloader {
public:
<<<<<<< HEAD
    VectorTileDownloader(
        std::string destination,
        const plateau::geometry::Extent& extent,
        int zoom_level = 15);

    void setExtent(const plateau::geometry::Extent& extent);

    int getTileCount() const;
    TileCoordinate getTile(int index) const;
    std::shared_ptr<VectorTile> download(int index) const;
    void download(int index, VectorTile& out_vector_tile) const;

    const std::string& getUrl();
    void setUrl(const std::string& value);

    static const std::string& getDefaultUrl();
    static void download(const std::string& url, const std::string& destination, const TileCoordinate& coordinate, VectorTile& out_vector_tile);

private:
    static inline std::string default_url_ = "http://cyberjapandata.gsi.go.jp/xyz/std/{z}/{x}/{y}.png";

    std::string url_;
    std::string destination_;
    plateau::geometry::Extent extent_;
    int zoom_level_;
    std::shared_ptr<std::vector<TileCoordinate>> tiles_;

    void updateTileCoordinates();
=======
    VectorTileDownloader(const char* uri);

    std::shared_ptr <VectorTile> download(const char* destination, TileCoordinate coordinate);
    const char* getUri();
    void setUri(char* uri);

private:
    std::string uri_ = "http://cyberjapandata.gsi.go.jp/xyz/std/{z}/{x}/{y}.png";
>>>>>>> d66b8b0fa4c0cda7d16be1403f77b2a569ede64a
};
