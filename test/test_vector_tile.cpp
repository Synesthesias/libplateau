#include <gtest/gtest.h>

#include <plateau/basemap/VectorTileLoader.h>
#include <fstream>

class VectorTileTest : public ::testing::Test {
protected:

};

TEST_F(VectorTileTest, VectorTileTest) {
    VectorTileLoader loader;
    plateau::geometry::GeoCoordinate min(35.1, 137, 0);
    plateau::geometry::GeoCoordinate max(35, 137.1, 0);
    plateau::geometry::Extent extent(min, max);

    auto vectorTiles = loader.load(extent);

    int i = 1;
    for (auto vectorTile : *vectorTiles)
    {
        std::string extension = ".png";
        std::string foldPath = "D:\\data\\test\\";

        std::string filePath = foldPath + std::to_string(vectorTile.coordinate.zoom_level)
            + "-" + std::to_string(vectorTile.coordinate.column) + "-"
            + std::to_string(vectorTile.coordinate.row) + extension;
        auto image = vectorTile.image.c_str();
        std::fstream fs(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
        fs.write(image, vectorTile.image.length());
        i++;
    }

    ASSERT_EQ(vectorTiles->size(), 120);
}

