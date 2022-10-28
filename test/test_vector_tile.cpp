#include <gtest/gtest.h>

#include <plateau/basemap/VectorTileDownloader.h>
#include <plateau/basemap/TileProjection.h>
#include <filesystem>


class VectorTileTest : public ::testing::Test {
protected:
};

TEST_F(VectorTileTest, VectorTileTest) {
    plateau::geometry::GeoCoordinate min(35.5335751, 139.7755041, -10000);
    plateau::geometry::GeoCoordinate max(35.54136964, 139.78712557, 10000);
    plateau::geometry::Extent extent(min, max);

    std::string destination = "./Basemap";
    VectorTileDownloader downloader(destination, extent);

    ASSERT_EQ(downloader.getTileCount(), 4);
    
    auto coordinate = downloader.getTile(0);

    ASSERT_EQ(coordinate.zoom_level, 15);
    ASSERT_EQ(coordinate.column, 29106);
    ASSERT_EQ(coordinate.row, 12918);

    coordinate = downloader.getTile(1);

    ASSERT_EQ(coordinate.zoom_level, 15);
    ASSERT_EQ(coordinate.column, 29106);
    ASSERT_EQ(coordinate.row, 12919);

    auto tile = downloader.download(0);

    ASSERT_EQ(tile.coordinate.zoom_level, 15);
    ASSERT_EQ(tile.coordinate.column, 29106);
    ASSERT_EQ(tile.coordinate.row, 12918);
    ASSERT_EQ(tile.image_path, destination + "\\15\\29106\\12918.png");
    ASSERT_TRUE(std::filesystem::exists(tile.image_path));
    std::filesystem::remove_all(destination);
}
