#include <plateau/basemap/vector_tile_downloader.h>
#include <plateau/basemap/tile_projection.h>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <filesystem>
#include <fstream>


namespace fs = std::filesystem;

const std::string VectorTileDownloader::default_url_ = "https://cyberjapandata.gsi.go.jp/xyz/std/{z}/{x}/{y}.png";

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

    const auto path = expandMapUrlTemplate(url_template, coordinate);
    const auto host = calcHost(path);

    httplib::Client client(host);
    std::string body;
    client.set_connection_timeout(5, 0);
    client.enable_server_certificate_verification(false);

    auto result = client.Get(
        path, httplib::Headers(),
        [&](const httplib::Response& response) {
            return true; // return 'false' if you want to cancel the request.
        },
        [&](const char* data, size_t data_length) {
            body.append(data, data_length);
            return true; // return 'false' if you want to cancel the request.
        });

    if (result.error() != httplib::Error::Success) {
        out_vector_tile.image_path.clear();
        out_vector_tile.result = static_cast<HttpResult>(result.error());
        return;
    }

    auto file_path = calcDestinationPath(coordinate, destination);
    create_directories(file_path.parent_path());
    std::fstream fs(file_path, std::ios::out | std::ios::binary | std::ios::trunc);
    fs.write(body.c_str(), body.length());

    out_vector_tile.image_path = file_path.u8string();
}

std::shared_ptr<VectorTile> VectorTileDownloader::download(const std::string& url_template, const std::string& destination,
                                                           const TileCoordinate& coordinate) {
    auto result = std::make_shared<VectorTile>();
    download(url_template, destination, coordinate, *result);
    return result;
}

fs::path VectorTileDownloader::calcDestinationPath(const TileCoordinate& coord, const std::string& destination) {
    const std::string extension = ".png";
    auto folder_path =
        fs::u8path(destination)
        .append(std::to_string(coord.zoom_level))
        .append(std::to_string(coord.column));
    create_directories(folder_path);
    auto file_path = folder_path.append(std::to_string(coord.row) + extension);
    return file_path;
}

std::filesystem::path VectorTileDownloader::calcDestinationPath(int index) const {
    return calcDestinationPath(tiles_->at(index), destination_);
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

const std::string& VectorTileDownloader::getUrl() {
    return url_;
}

void VectorTileDownloader::setUrl(const std::string& value) {
    url_ = value;
}

const std::string& VectorTileDownloader::getDefaultUrl() {
    return default_url_;
}
