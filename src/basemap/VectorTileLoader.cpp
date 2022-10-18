#include <plateau/basemap/VectorTileLoader.h>
#include <plateau/basemap/TileProjection.h>
#include <plateau/basemap/VectorTileDownloader.h>
#include <memory>
#include <vector>

#include <plateau/geometry/geo_coordinate.h>


std::shared_ptr<std::vector<VectorTile>> VectorTileLoader::load(plateau::geometry::Extent extent) {
    std::vector<VectorTile> vectorTiles_;
    auto vectorTiles = std::make_shared<std::vector<VectorTile>>();

    TileProjection tileProjection;
    auto temp = tileProjection.getTileCoordinates(extent);
    for (TileCoordinate coordinate : *temp) {
        VectorTileDownloader downloader;
        vectorTiles->push_back(downloader.download(uri_, coordinate));
    }

    return vectorTiles;
}

char* VectorTileLoader::getUri() {
    std::string tmp = uri_;
    char uri_c[256];
    memcpy(uri_c, uri_.c_str(), 256);

    return uri_c;
}
