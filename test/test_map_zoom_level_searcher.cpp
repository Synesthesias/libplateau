#include <gtest/gtest.h>
#include <plateau/texture/map_zoom_level_searcher.h>

namespace plateau::texture {
    using namespace geometry;

    class MapZoomLevelSearcherTest : public ::testing::Test {
    public:
        static const GeoCoordinate coord;
    };
    const GeoCoordinate MapZoomLevelSearcherTest::coord = GeoCoordinate(35.6771964003015, 139.74784608229484, 0);

    TEST_F(MapZoomLevelSearcherTest, ZoomLevelOfGSIAerialPhoto) { // NOLINT
        static const std::string url_template = "https://cyberjapandata.gsi.go.jp/xyz/seamlessphoto/{z}/{x}/{y}.jpg";
        const auto result = MapZoomLevelSearcher::search(url_template, coord);
        EXPECT_EQ(result.available_zoom_level_max_, 18);
        EXPECT_EQ(result.available_zoom_level_min_, 2);
        EXPECT_TRUE(result.is_valid_);
    }

    TEST_F(MapZoomLevelSearcherTest, ZoomLevelOfReliefHeightMap) { // NOLINT
        static const std::string url_template = "https://cyberjapandata.gsi.go.jp/xyz/relief/{z}/{x}/{y}.png";
        const auto result = MapZoomLevelSearcher::search(url_template, coord);
        EXPECT_EQ(result.available_zoom_level_max_, 15);
        EXPECT_EQ(result.available_zoom_level_min_, 5);
        EXPECT_TRUE(result.is_valid_);
    }

    TEST_F(MapZoomLevelSearcherTest, ResultIsInvalidWhenUrlIsInvalid) { // NOLINT
        static const std::string url_template = "https://invalid_url.com/{x}/{y}/{z}.png";
        const auto result = MapZoomLevelSearcher::search(url_template, coord);
        EXPECT_FALSE(result.is_valid_);
    }
}
