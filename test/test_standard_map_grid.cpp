#include <gtest/gtest.h>

#include <citygml/citygml.h>

#include <plateau/geometry/geo_coordinate.h>
#include <plateau/dataset/standard_map_grid.h>

using namespace citygml;
using namespace plateau::dataset;
using namespace plateau::geometry;

TEST(StandardMapGrid, Level5000_WithinBounds) {
    const auto extent = StandardMapGrid("08EE54").getExtent();    // Level5000
    const auto expected = GeoCoordinate(37.4689, 138.7113, 0);

    std::stringstream ss;
    ss << "Extent bounds: (" << extent.min.latitude << ", " << extent.min.longitude << ") - ("
       << extent.max.latitude << ", " << extent.max.longitude << ")\n"
       << "Expected point: (" << expected.latitude << ", " << expected.longitude << ")";
    SCOPED_TRACE(ss.str());
    ASSERT_TRUE(
        extent.max.latitude >= expected.latitude &&
        extent.max.longitude >= expected.longitude &&
        extent.min.latitude <= expected.latitude &&
        extent.min.longitude <= expected.longitude
    );
}

TEST(StandardMapGrid, Level5000_OutOfBounds) {
    const auto extent = StandardMapGrid("08EE54").getExtent();    // Level5000
    const auto nonexpected = GeoCoordinate(37.4659, 138.7283, 0);

    std::stringstream ss;
    ss << "Extent bounds: (" << extent.min.latitude << ", " << extent.min.longitude << ") - ("
       << extent.max.latitude << ", " << extent.max.longitude << ")\n"
       << "Non-expected point: (" << nonexpected.latitude << ", " << nonexpected.longitude << ")";
    SCOPED_TRACE(ss.str());
    ASSERT_FALSE(
        extent.max.latitude >= nonexpected.latitude &&
        extent.max.longitude >= nonexpected.longitude &&
        extent.min.latitude <= nonexpected.latitude &&
        extent.min.longitude <= nonexpected.longitude
    );
}

TEST(StandardMapGrid, Level2500_WithinBounds) {
    const auto extent = StandardMapGrid("08EE554").getExtent();   // Level2500
    const auto expected = GeoCoordinate(37.4661, 138.7678, 0);

    std::stringstream ss;
    ss << "Extent bounds: (" << extent.min.latitude << ", " << extent.min.longitude << ") - ("
       << extent.max.latitude << ", " << extent.max.longitude << ")\n"
       << "Expected point: (" << expected.latitude << ", " << expected.longitude << ")";
    SCOPED_TRACE(ss.str());
    ASSERT_TRUE(
        extent.max.latitude >= expected.latitude &&
        extent.max.longitude >= expected.longitude &&
        extent.min.latitude <= expected.latitude &&
        extent.min.longitude <= expected.longitude
    );
}

TEST(StandardMapGrid, Level2500_OutOfBounds) {
    const auto extent = StandardMapGrid("08EE554").getExtent();   // Level2500
    const auto nonexpected = GeoCoordinate(37.4631, 138.7848, 0);

    std::stringstream ss;
    ss << "Extent bounds: (" << extent.min.latitude << ", " << extent.min.longitude << ") - ("
       << extent.max.latitude << ", " << extent.max.longitude << ")\n"
       << "Non-expected point: (" << nonexpected.latitude << ", " << nonexpected.longitude << ")";
    SCOPED_TRACE(ss.str());
    ASSERT_FALSE(
        extent.max.latitude >= nonexpected.latitude &&
        extent.max.longitude >= nonexpected.longitude &&
        extent.min.latitude <= nonexpected.latitude &&
        extent.min.longitude <= nonexpected.longitude
    );
}

TEST(StandardMapGrid, isWithinTest) {
    // Level50000とLevel5000の包含関係
    const auto grid50000 = StandardMapGrid("09AE");
    const auto grid5000 = StandardMapGrid("09AE09");
    const auto grid2500 = StandardMapGrid("09AE091");
    const auto grid1000 = StandardMapGrid("09AE091A");
    const auto grid500 = StandardMapGrid("09AE0911");

    // 同じコードは包含関係にある
    ASSERT_TRUE(grid50000.isWithin(grid50000));
    ASSERT_TRUE(grid5000.isWithin(grid5000));

    // Level50000は下位レベルを包含する
    ASSERT_TRUE(grid50000.isWithin(grid5000));
    ASSERT_TRUE(grid50000.isWithin(grid2500));
    ASSERT_TRUE(grid50000.isWithin(grid1000));
    ASSERT_TRUE(grid50000.isWithin(grid500));

    // Level5000は下位レベルを包含する
    ASSERT_TRUE(grid5000.isWithin(grid2500));
    ASSERT_TRUE(grid5000.isWithin(grid1000));
    ASSERT_TRUE(grid5000.isWithin(grid500));

    // 下位レベルは上位レベルを包含しない
    ASSERT_FALSE(grid5000.isWithin(grid50000));
    ASSERT_FALSE(grid2500.isWithin(grid50000));
    ASSERT_FALSE(grid2500.isWithin(grid5000));
}

TEST(StandardMapGrid, invalidGridCode) {
    // 無効な図郭コードのテスト
    ASSERT_FALSE(StandardMapGrid("invalid").isValid());
    ASSERT_FALSE(StandardMapGrid("09").isValid());        // 短すぎる
    ASSERT_FALSE(StandardMapGrid("09AE09-1-1").isValid()); // 長すぎる
    ASSERT_FALSE(StandardMapGrid("09ZZ09").isValid());    // 無効な文字
}

TEST(StandardMapGrid, levelCheck) {
    // 各レベルの判定テスト
    ASSERT_EQ(StandardMapGrid("09AE").getLevel(), 0);      // Level50000
    ASSERT_EQ(StandardMapGrid("09AE09").getLevel(), 1);    // Level5000
    ASSERT_EQ(StandardMapGrid("09AE091").getLevel(), 2);  // Level2500
    ASSERT_EQ(StandardMapGrid("09AE091A").getLevel(), 3); // Level1000
    ASSERT_EQ(StandardMapGrid("09AE0911").getLevel(), 4); // Level500
} 