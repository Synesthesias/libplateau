#include <gtest/gtest.h>

#include <plateau/basemap/VectorTileDownloader.h>
#include <plateau/basemap/TileProjection.h>


class VectorTileTest : public ::testing::Test {
protected:
};

TEST_F(VectorTileTest, VectorTileTest) {
    plateau::geometry::GeoCoordinate min(35.5335751, 139.7755041, -10000);
    plateau::geometry::GeoCoordinate max(35.54136964, 139.78712557, 10000);
    plateau::geometry::Extent extent(min, max);

    TileProjection tileProjection;
    auto tileCoordinates = tileProjection.getTileCoordinates(extent, 15);

    ASSERT_EQ(tileCoordinates->size(), 4);
}
