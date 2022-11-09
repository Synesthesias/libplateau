#pragma once

#include <memory>
#include <libplateau_api.h>
#include <filesystem>
#include "plateau/geometry/geo_coordinate.h"


struct TileCoordinate {
    int column;
    int row;
    int zoom_level;
};

struct VectorTile {
    TileCoordinate coordinate{};
    std::string image_path;
};

/**
 * 地理院地図タイルをダウンロードして画像ファイルとして保存します。
 */
class LIBPLATEAU_EXPORT VectorTileDownloader {
public:
    VectorTileDownloader(
        std::string destination,
        const plateau::geometry::Extent& extent,
        int zoom_level = 15);

    void setExtent(const plateau::geometry::Extent& extent);

    int getTileCount() const;
    TileCoordinate getTile(int index) const;
    std::shared_ptr<VectorTile> download(int index) const;
    void download(int index, VectorTile& out_vector_tile) const;

    /// TileCoordinateの地図タイルをダウンロードしたとき、その画像ファイルがどこに配置されるべきかを返します。
    static std::filesystem::path calcDestinationPath(const TileCoordinate& coord, const std::string& destination);
    std::filesystem::path calcDestinationPath(int index) const;

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
};
