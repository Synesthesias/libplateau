#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <plateau/basemap/vector_tile_downloader.h>
#include <plateau/basemap/tile_projection.h>
#include <filesystem>
#include <fstream>


namespace fs = std::filesystem;
using namespace plateau::geometry;

const std::string VectorTileDownloader::default_url_ = "https://cyberjapandata.gsi.go.jp/xyz/std/{z}/{x}/{y}.png";

VectorTiles::VectorTiles(std::vector<VectorTile> tiles) :
        tiles_(std::move(tiles)),
        min_column_(INT_MAX), min_row_(INT_MAX), max_column_(INT_MIN), max_row_(INT_MIN),
        zoom_level_(0){
    // 最小最大を求めます。
    for(const auto& tile : tiles_) {
        const auto coord = tile.coordinate;
        min_column_ = std::min(min_column_, coord.column);
        min_row_ = std::min(min_row_, coord.row);
        max_column_ = std::max(max_column_, coord.column);
        max_row_ = std::max(max_row_, coord.row);
    }
    if(!tiles_.empty()) {
        zoom_level_ = tiles_.at(0).coordinate.zoom_level;
    }
}

bool VectorTiles::anyTileSucceed() {
    if(tiles_.empty()) return false;
    for(const auto& tile : tiles_) {
        if(tile.result == HttpResult::Success) {
            return true;
        }
    }
    return false;
}

const VectorTile& VectorTiles::firstSucceed() const {
    for(const auto& tile : tiles_) {
        if(tile.result == HttpResult::Success) {
            return tile;
        }
    }
    throw std::runtime_error("succeed tile is not found.");
}

const VectorTile& VectorTiles::getTile(int column, int row) const {
    for(const auto& tile : tiles_) {
        auto coord = tile.coordinate;
        if(coord.column == column && coord.row == row) return tile;
    }
    throw std::runtime_error("tile not found.");
}

Extent VectorTiles::extent() const {
    const auto min_col = minColumn();
    const auto max_col = maxColumn();
    const auto min_row = minRow();
    const auto max_row = maxRow();
    const auto zoom = zoomLevel();

    // 緯度は北が正、地図タイルは南が正です。南北方向が逆であることに注意してください。
    const auto min_tile = TileCoordinate(min_col, max_row, zoom);
    const auto max_tile = TileCoordinate(max_col, min_row, zoom);

    const auto min_geo = TileProjection::unproject(min_tile).min;
    const auto max_geo = TileProjection::unproject(max_tile).max;
    return {min_geo, max_geo};
}

VectorTileDownloader::VectorTileDownloader(
    const std::string& destination,
    const plateau::geometry::Extent& extent,
    const int zoom_level)
    : url_(default_url_)
    , destination_(destination)
    , extent_(extent)
    , zoom_level_(zoom_level)
    , tiles_(TileProjection::getTileCoordinates(extent, zoom_level)) {
    setExtent(extent);
}


namespace {
    void replaceStr(std::string& str, const std::string& from, const std::string& to) {
        const auto pos = str.find(from);
        const auto len = from.length();
        if(pos == std::string::npos || from.empty()) return;
        str.replace(pos, len, to);
    }

    /**
     * URLに含まれる{x},{y},{z}をTileCoordinateの数値に置き換えて返します。
     */
    std::string expandMapUrlTemplate(const std::string& map_url_template,const TileCoordinate tile) {
        auto url = map_url_template;
        replaceStr(url, "{z}", std::to_string(tile.zoom_level));
        replaceStr(url, "{y}", std::to_string(tile.row));
        replaceStr(url, "{x}", std::to_string(tile.column));
        return url;
    }

    /// URLからhostを取り出します。
    std::string calcHost(const std::string& url) {
        const auto http_pos = url.find("http");
        if(http_pos == std::string::npos) {
            throw std::runtime_error("Url does not start with http");
        }
        const auto double_slash_pos = url.find("//");
        if(double_slash_pos == std::string::npos) {
            throw std::runtime_error("Url does not contain //");
        }
        auto slash_pos = url.find('/', double_slash_pos + 2);
        if(slash_pos == std::string::npos) {
            slash_pos = url.length();
        }
        return url.substr(0, slash_pos);
    }

}

void VectorTileDownloader::download(
    const std::string& url_template,
    const std::string& destination,
    const TileCoordinate& coordinate,
    VectorTile& out_vector_tile
) {
    out_vector_tile.coordinate = coordinate;

    std::string body;
    auto result = httpRequest(url_template, coordinate, body);

    if (result.error() != httplib::Error::Success || result->status != 200) {
        out_vector_tile.image_path.clear();
        out_vector_tile.result = static_cast<HttpResult>(result.error());
        return;
    }

    const std::string file_extension = fs::u8path(url_template).extension().u8string();
    const auto file_path = calcDestinationPath(coordinate, destination, file_extension);
    create_directories(file_path.parent_path());
    std::fstream fs(file_path, std::ios::out | std::ios::binary | std::ios::trunc);
    fs.write(body.c_str(), (std::streamsize)body.length());

    out_vector_tile.image_path = file_path.u8string();
}

httplib::Result VectorTileDownloader::httpRequest(const std::string& url_template, TileCoordinate tile_coordinate, std::string& out_body) {
    const auto path = expandMapUrlTemplate(url_template, tile_coordinate);
    const auto host = calcHost(path);

    httplib::Client client(host);
    client.set_connection_timeout(1, 0);
    client.enable_server_certificate_verification(false);

    auto result = client.Get(
            path, httplib::Headers(),
            [&](const httplib::Response& response) {
                return true; // return 'false' if you want to cancel the request.
            },
            [&](const char* data, size_t data_length) {
                out_body.append(data, data_length);
                return true; // return 'false' if you want to cancel the request.
            });
    return result;
}

std::shared_ptr<VectorTile> VectorTileDownloader::download(const std::string& url_template, const std::string& destination,
                                                           const TileCoordinate& coordinate) {
    auto result = std::make_shared<VectorTile>();
    download(url_template, destination, coordinate, *result);
    return result;
}

VectorTiles VectorTileDownloader::downloadAll() const {
    auto tiles = std::vector<VectorTile>();
    auto count = getTileCount();
    for(int i=0; i<count; i++) {
        auto tile = download(i);
        tiles.push_back(*tile);
    }
    auto ret = VectorTiles(tiles);
    return ret;
}

fs::path VectorTileDownloader::calcDestinationPath(const TileCoordinate& coord, const std::string& destination, const std::string& file_extension) {
    auto folder_path =
        fs::u8path(destination)
        .append(std::to_string(coord.zoom_level))
        .append(std::to_string(coord.column));
    create_directories(folder_path);
    auto file_path = folder_path.append(std::to_string(coord.row) + file_extension);
    return file_path;
}

std::filesystem::path VectorTileDownloader::calcDestinationPath(int index) const {
    const std::string file_extension = fs::u8path(getUrl()).extension().u8string();
    return calcDestinationPath(tiles_->at(index), destination_, file_extension);
}

void VectorTileDownloader::updateTileCoordinates() {
    tiles_ = TileProjection::getTileCoordinates(extent_, zoom_level_);
}

void VectorTileDownloader::setExtent(const plateau::geometry::Extent& extent) {
    extent_ = extent;

    updateTileCoordinates();
}

int VectorTileDownloader::getTileCount() const {
    if (tiles_ == nullptr)
        return 0;

    return static_cast<int>(tiles_->size());
}

TileCoordinate VectorTileDownloader::getTile(int index) {
    if (tiles_ == nullptr)
        updateTileCoordinates();

    return tiles_->at(index);
}

std::shared_ptr<VectorTile> VectorTileDownloader::download(const int index) const {
    if (tiles_ == nullptr)
        return nullptr;

    const auto result = std::make_shared<VectorTile>();
    download(url_, destination_, tiles_->at(index), *result);

    return result;
}

bool VectorTileDownloader::download(int index, VectorTile& out_vector_tile) const {
    if (tiles_ == nullptr){
        out_vector_tile = {};
        return false;
    }

    download(url_, destination_, tiles_->at(index), out_vector_tile);
    return true;
}

const std::string& VectorTileDownloader::getUrl() const {
    return url_;
}

void VectorTileDownloader::setUrl(const std::string& value) {
    url_ = value;
}

const std::string& VectorTileDownloader::getDefaultUrl() {
    return default_url_;
}
