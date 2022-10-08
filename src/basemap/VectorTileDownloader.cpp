#include <plateau/basemap/VectorTileDownloader.h>
#include <httplib.h>


VectorTile VectorTileDownloader::download(
    std::string uri,
    TileCoordinate coordinate
    ) {
    VectorTile vector_tile;
    vector_tile.coordinate = coordinate;

    //URI編集
    int first = 0;
    int last = uri.find_first_of('/');
    std::vector<std::string> strs;

    while (first < uri.size()) {
        std::string subStr(uri, first, last - first);
        strs.push_back(subStr);

        first = last + 1;
        last = uri.find_first_of('/', first);

        if (last == std::string::npos) {
            last = uri.size();
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
    vector_tile.image = body;


    return vector_tile;
}
