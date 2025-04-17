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

TEST(StandardMapGrid, upperMethodTest) {
    // Level500からLevel50000までの変換をテスト
    auto grid = StandardMapGrid("09AE0911");  // Level500
    ASSERT_EQ(grid.upper()->get(), "09AE091"); // Level1000
    
    grid = StandardMapGrid("09AE091A");  // Level1000
    ASSERT_EQ(grid.upper()->get(), "09AE091"); // Level2500
    
    grid = StandardMapGrid("09AE091");  // Level2500
    ASSERT_EQ(grid.upper()->get(), "09AE09"); // Level5000
    
    grid = StandardMapGrid("09AE09");  // Level5000
    ASSERT_EQ(grid.upper()->get(), "09AE"); // Level50000
    
    // Level50000からの変換は無効になるはず
    grid = StandardMapGrid("09AE");  // Level50000
    auto upper_grid = grid.upper();
    ASSERT_FALSE(upper_grid->isValid());
}

TEST(StandardMapGrid, levelCheckMethods) {
    // isLargestLevelのテスト
    ASSERT_TRUE(StandardMapGrid("09AE").isLargestLevel());      // Level50000
    ASSERT_FALSE(StandardMapGrid("09AE09").isLargestLevel());   // Level5000
    
    // isSmallerThanNormalGmlのテスト
    ASSERT_TRUE(StandardMapGrid("09AE").isSmallerThanNormalGml());   // Level50000
    ASSERT_TRUE(StandardMapGrid("09AE09").isSmallerThanNormalGml()); // Level5000
    ASSERT_FALSE(StandardMapGrid("09AE091").isSmallerThanNormalGml()); // Level2500
    ASSERT_FALSE(StandardMapGrid("09AE091A").isSmallerThanNormalGml()); // Level1000
    
    // isNormalGmlLevelのテスト
    ASSERT_FALSE(StandardMapGrid("09AE").isNormalGmlLevel());     // Level50000
    ASSERT_FALSE(StandardMapGrid("09AE09").isNormalGmlLevel());   // Level5000
    ASSERT_TRUE(StandardMapGrid("09AE091").isNormalGmlLevel());   // Level2500
    ASSERT_FALSE(StandardMapGrid("09AE091A").isNormalGmlLevel()); // Level1000
} 