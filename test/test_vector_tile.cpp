#include <gtest/gtest.h>

#include <plateau/basemap/VectorTileLoader.h>
#include <fstream>

class VectorTileTest : public ::testing::Test {
protected:
};

TEST_F(VectorTileTest, VectorTileTest) {
    VectorTileLoader loader;
    plateau::geometry::GeoCoordinate min(35, 137, 0);
    plateau::geometry::GeoCoordinate max(35.1, 137.1, 0);
    plateau::geometry::Extent extent(min, max);

    auto vectorTiles = loader.load(extent);

    ASSERT_EQ(vectorTiles->size(), 120);
}

