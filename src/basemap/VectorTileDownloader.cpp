#include <plateau/basemap/VectorTileDownloader.h>
#include <plateau/basemap/TileProjection.h>

#include <httplib.h>
#include <filesystem>
#include <fstream>
#include <utility>

namespace fs = std::filesystem;

VectorTileDownloader::VectorTileDownloader(
    std::string destination,
    const plateau::geometry::Extent& extent,
    const int zoom_level)
    : url_(default_url_)
    , destination_(std::move(destination))
    , extent_(extent)
    , zoom_level_(zoom_level)
    , tiles_(TileProjection::getTileCoordinates(extent, zoom_level)) {
}


void VectorTileDownloader::download(
    const std::string& url,
    const std::string& destination,
    const TileCoordinate& coordinate,
    VectorTile& out_vector_tile
) {
    out_vector_tile.coordinate = coordinate;

    //URI編集
    size_t first = 0;
    auto last = url.find_first_of('/');
    std::vector<std::string> strs;

    while (first < url.size()) {
        std::string subStr(url, first, last - first);
        strs.push_back(subStr);

        first = last + 1ull;
        last = url.find_first_of('/', first);

        if (last == std::string::npos) {
            last = url.size();
        }
    }

    std::string host = strs[0] + "/" + strs[1] + "/" + strs[2];
    std::string path = "/" + strs[3] + "/" + strs[4] + "/" + std::to_string(coordinate.zoom_level) + "/" + std::to_string(coordinate.column) + "/" + std::to_string(coordinate.row) + ".png";


    httplib::Client client(host);
    std::string body;
    auto result = client.Get(
        path, httplib::Headers(),
        [&](const httplib::Response& response) {
            return true; // return 'false' if you want to cancel the request.
        },
        [&](const char* data, size_t data_length) {
            body.append(data, data_length);
            return true; // return 'false' if you want to cancel the request.
        });

    const std::string extension = ".png";
    auto folder_path =
        fs::u8path(destination)
        .append(std::to_string(coordinate.zoom_level))
        .append(std::to_string(coordinate.column));
    create_directories(folder_path);
    auto file_path = folder_path.append(std::to_string(coordinate.row) + extension);
    std::fstream fs(file_path, std::ios::out | std::ios::binary | std::ios::trunc);
    fs.write(body.c_str(), body.length());

    out_vector_tile.image_path = file_path.u8string();
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

TileCoordinate VectorTileDownloader::getTile(int index) const {
    if (tiles_ == nullptr)
        return {};

    return (*tiles_)[index];
}

std::shared_ptr<VectorTile> VectorTileDownloader::download(const int index) const {
    if (tiles_ == nullptr)
        return nullptr;

    const auto result = std::make_shared<VectorTile>();
    download(url_, destination_, (*tiles_)[index], *result);

    return result;
}

void VectorTileDownloader::download(int index, VectorTile& out_vector_tile) const {
    if (tiles_ == nullptr)
        out_vector_tile = {};

    download(url_, destination_, (*tiles_)[index], out_vector_tile);
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
