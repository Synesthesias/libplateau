#include <gtest/gtest.h>

#include <plateau/basemap/vector_tile_downloader.h>
#include <plateau/basemap/tile_projection.h>
#include "../src/c_wrapper/vector_tile_downloader_c.cpp"
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

    VectorTile tile;
    downloader.download(0, tile);

    ASSERT_EQ(tile.coordinate.zoom_level, 15);
    ASSERT_EQ(tile.coordinate.column, 29106);
    ASSERT_EQ(tile.coordinate.row, 12918);
    ASSERT_EQ(tile.image_path, std::filesystem::u8path(destination).append("15").append("29106").append("12918.png").u8string());
    ASSERT_EQ(tile.image_path, downloader.calcDestinationPath(0));

    ASSERT_TRUE(std::filesystem::exists(tile.image_path));
    std::filesystem::remove_all(destination);

    auto actual_extent = TileProjection::unproject(tile.coordinate);

    ASSERT_LE(abs(actual_extent.min.latitude - 35.5414), 0.001);
    ASSERT_LE(abs(actual_extent.min.longitude - 139.768), 0.001);
    ASSERT_LE(abs(actual_extent.max.latitude - 35.5501), 0.001);
    ASSERT_LE(abs(actual_extent.max.longitude - 139.779), 0.001);
}
