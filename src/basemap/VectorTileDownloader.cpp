#include <plateau/basemap/VectorTileDownloader.h>
#include <plateau/basemap/TileProjection.h>

#include <httplib.h>
#include  <filesystem>
#include <fstream>


VectorTileDownloader::VectorTileDownloader(const char* uri) {
    uri_ = uri;
}

std::shared_ptr <VectorTile> VectorTileDownloader::download(
    const char* destination,
    TileCoordinate coordinate
) {
    auto vector_tile = std::make_shared<VectorTile>();
    vector_tile->coordinate = coordinate;

    //URI編集
    int first = 0;
    int last = uri_.find_first_of('/');
    std::vector<std::string> strs;

    while (first < uri_.size()) {
        std::string subStr(uri_, first, last - first);
        strs.push_back(subStr);

        first = last + 1;
        last = uri_.find_first_of('/', first);

        if (last == std::string::npos) {
            last = uri_.size();
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

    std::string extension = ".png";
    std::string dest = destination;
    std::string foldPath = dest + "\\" + std::to_string(coordinate.zoom_level) + "\\"
        + std::to_string(coordinate.column);
    std::filesystem::create_directories(foldPath);
    std::string filePath = foldPath + "\\" + std::to_string(coordinate.row) + extension;
    std::fstream fs(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
    fs.write(body.c_str(), body.length());

    vector_tile->image_path = filePath;

    return vector_tile;
}

const char* VectorTileDownloader::getUri() {
    return uri_.c_str();
}

void VectorTileDownloader::setUri(char* uri) {
    uri_ = uri;
}
