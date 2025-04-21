#include <gtest/gtest.h>

#include <citygml/citygml.h>

#include <plateau/geometry/geo_coordinate.h>
#include <plateau/dataset/standard_map_grid.h>

using namespace citygml;
using namespace plateau::dataset;
using namespace plateau::geometry;

TEST(StandardMapGrid, Level50000_WithinBounds) {
    const auto extent = StandardMapGrid("08JE").getExtent();
    const auto expected = GeoCoordinate(36.0935, 138.8013, 0);

    const auto extent2 = StandardMapGrid("08KD").getExtent();
    const auto expected2 = GeoCoordinate(35.7532, 138.0981, 0);

    const auto extent3 = StandardMapGrid("08KC").getExtent();
    const auto expected3 = GeoCoordinate(35.7978, 137.8043, 0);

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

    std::stringstream ss2;
    ss2 << "Extent2 bounds: (" << extent2.min.latitude << ", " << extent2.min.longitude << ") - ("
        << extent2.max.latitude << ", " << extent2.max.longitude << ")\n"
        << "Expected point2: (" << expected2.latitude << ", " << expected2.longitude << ")";
    SCOPED_TRACE(ss2.str());
    ASSERT_TRUE(
        extent2.max.latitude >= expected2.latitude &&
        extent2.max.longitude >= expected2.longitude &&
        extent2.min.latitude <= expected2.latitude &&
        extent2.min.longitude <= expected2.longitude
    );

    std::stringstream ss3;
    ss3 << "Extent3 bounds: (" << extent3.min.latitude << ", " << extent3.min.longitude << ") - ("
        << extent3.max.latitude << ", " << extent3.max.longitude << ")\n"
        << "Expected point3: (" << expected3.latitude << ", " << expected3.longitude << ")";
    SCOPED_TRACE(ss3.str());
    ASSERT_TRUE(
        extent3.max.latitude >= expected3.latitude &&
        extent3.max.longitude >= expected3.longitude &&
        extent3.min.latitude <= expected3.latitude &&
        extent3.min.longitude <= expected3.longitude
    );
}

TEST(StandardMapGrid, Level5000_WithinBounds) {
    const auto extent = StandardMapGrid("08JE54").getExtent();    // Level5000
    const auto expected = GeoCoordinate(36.1125, 138.6911, 0);

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

TEST(StandardMapGrid, Level2500_WithinBounds) {
    const auto extent = StandardMapGrid("08EE554").getExtent();
    const auto expected = GeoCoordinate(37.4669, 138.7544, 0);

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

TEST(StandardMapGrid, Level1000_WithinBounds) {
    const auto extent = StandardMapGrid("08JE640E").getExtent();
    const auto expected = GeoCoordinate(36.1053, 138.7151, 0);

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

TEST(StandardMapGrid, invalidGridCode) {
    // 無効な図郭コードのテスト
    ASSERT_FALSE(StandardMapGrid("invalid").isValid());
    ASSERT_FALSE(StandardMapGrid("09").isValid());        // 短すぎる
    ASSERT_FALSE(StandardMapGrid("09AE09111").isValid()); // 長すぎる
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
    ASSERT_EQ(grid.upper()->get(), "09AE09"); // Level5000
    
    grid = StandardMapGrid("09AE091A");  // Level1000
    ASSERT_EQ(grid.upper()->get(), "09AE09"); // Level5000
    
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

TEST(StandardMapGrid, calculateGridExtent_Level50000) {
    // 基準点のテスト
    {
        const auto grid = StandardMapGrid("08JE");
        const auto [min, max] = grid.calculateGridExtent();
        
        EXPECT_NEAR(min.x, 0, 0.1);
        EXPECT_NEAR(min.z, 0, 0.1);
        EXPECT_NEAR(max.x, 40000, 0.1);
        EXPECT_NEAR(max.z, 30000, 0.1);

        std::cout << "Grid 08JE: "
                  << "(" << min.x << ", " << min.z << ") - (" 
                  << max.x << ", " << max.z << ")" << std::endl;
    }

    {
        const auto grid = StandardMapGrid("08NA");  // N（南）, A（東西）
        const auto [min, max] = grid.calculateGridExtent();
        
        EXPECT_NEAR(min.x, -160000, 0.1);
        EXPECT_NEAR(min.z, -120000, 0.1);
        EXPECT_NEAR(max.x, -120000, 0.1);
        EXPECT_NEAR(max.z, -90000, 0.1);

        std::cout << "Grid 08NA: "
                  << "(" << min.x << ", " << min.z << ") - ("
                  << max.x << ", " << max.z << ")" << std::endl;
    }

    {
        const auto grid = StandardMapGrid("08FA");  // F（北）, A（東西）
        const auto [min, max] = grid.calculateGridExtent();
        
        EXPECT_NEAR(min.x, -160000, 0.1);
        EXPECT_NEAR(min.z, 120000, 0.1);
        EXPECT_NEAR(max.x, -120000, 0.1);
        EXPECT_NEAR(max.z, 150000, 0.1);

        std::cout << "Grid 08FA: "
                  << "(" << min.x << ", " << min.z << ") - ("
                  << max.x << ", " << max.z << ")" << std::endl;
    }
}

TEST(StandardMapGrid, calculateGridExtent_Level5000) {
    // Level5000表面直角座標計算テスト
    {
        const auto grid = StandardMapGrid("08JE54");  // Level5000
        const auto [min, max] = grid.calculateGridExtent();
        
        EXPECT_NEAR(min.x, 16000.0, 0.1);
        EXPECT_NEAR(min.z, 12000.0, 0.1);
        EXPECT_NEAR(max.x, 20000.0, 0.1);
        EXPECT_NEAR(max.z, 15000.0, 0.1);

        std::cout << "\n08JE54:" << std::endl
                  << "(" << min.x << ", " << min.z << ") - (" << max.x << ", " << max.z << ")" << std::endl;
    }

    // Level5000表面直角座標計算テスト（別のケース）
    {
        const auto grid = StandardMapGrid("08NA54");  // Level5000
        const auto [min, max] = grid.calculateGridExtent();
        
        EXPECT_NEAR(min.x, -144000.0, 0.1);
        EXPECT_NEAR(min.z, -108000.0, 0.1);
        EXPECT_NEAR(max.x, -140000.0, 0.1);
        EXPECT_NEAR(max.z, -105000.0, 0.1);

        std::cout << "\n08NA54: "
                  << "(" << min.x << ", " << min.z << ") - ("
                  << max.x << ", " << max.z << ")" << std::endl;
    }
}

TEST(StandardMapGrid, calculateGridExtent_Level2500) {
    {
        const auto grid = StandardMapGrid("08JE541");
        const auto [min, max] = grid.calculateGridExtent();

        EXPECT_NEAR(min.x, 16000.0, 0.1);
        EXPECT_NEAR(min.z, 13500.0, 0.1);
        EXPECT_NEAR(max.x, 18000.0, 0.1);
        EXPECT_NEAR(max.z, 15000.0, 0.1);

        std::cout << "\n08JE54:" << std::endl
                  << "(" << min.x << ", " << min.z << ") - (" << max.x << ", " << max.z << ")" << std::endl;
    }

    {
        const auto grid = StandardMapGrid("08NA542");
        const auto [min, max] = grid.calculateGridExtent();

        EXPECT_NEAR(min.x, -142000.0, 0.1);
        EXPECT_NEAR(min.z, -106500.0, 0.1);
        EXPECT_NEAR(max.x, -140000.0, 0.1);
        EXPECT_NEAR(max.z, -105000.0, 0.1);

        std::cout << "\n08NA54: "
                  << "(" << min.x << ", " << min.z << ") - ("
                  << max.x << ", " << max.z << ")" << std::endl;
    }
}

TEST(StandardMapGrid, calculateGridExtent_Level1000) {
    {
        const auto grid = StandardMapGrid("08JE640E");
        const auto [min, max] = grid.calculateGridExtent();

        EXPECT_NEAR(min.x, 19200.0, 0.1);
        EXPECT_NEAR(min.z, 11400.0, 0.1);
        EXPECT_NEAR(max.x, 20000.0, 0.1);
        EXPECT_NEAR(max.z, 12000.0, 0.1);

        std::cout << "\n08JE54:" << std::endl
                  << "(" << min.x << ", " << min.z << ") - (" << max.x << ", " << max.z << ")" << std::endl;
    }

    {
        const auto grid = StandardMapGrid("08NA542B");
        const auto [min, max] = grid.calculateGridExtent();

        EXPECT_NEAR(min.x, -143200.0, 0.1);
        EXPECT_NEAR(min.z, -106800.0, 0.1);
        EXPECT_NEAR(max.x, -142400.0, 0.1);
        EXPECT_NEAR(max.z, -106200.0, 0.1);

        std::cout << "\n08NA54: "
                  << "(" << min.x << ", " << min.z << ") - ("
                  << max.x << ", " << max.z << ")" << std::endl;
    }
}

TEST(StandardMapGrid, calculateGridExtent_Level500) {
    {
        const auto grid = StandardMapGrid("08JE6421");
        const auto [min, max] = grid.calculateGridExtent();

        EXPECT_NEAR(min.x, 16400.0, 0.1);
        EXPECT_NEAR(min.z, 11100.0, 0.1);
        EXPECT_NEAR(max.x, 16800.0, 0.1);
        EXPECT_NEAR(max.z, 11400.0, 0.1);

        std::cout << "\n08JE54:" << std::endl
                  << "(" << min.x << ", " << min.z << ") - (" << max.x << ", " << max.z << ")" << std::endl;
    }

    {
        const auto grid = StandardMapGrid("08NA5476");
        const auto [min, max] = grid.calculateGridExtent();

        EXPECT_NEAR(min.x, -141600.0, 0.1);
        EXPECT_NEAR(min.z, -107400.0, 0.1);
        EXPECT_NEAR(max.x, -141200.0, 0.1);
        EXPECT_NEAR(max.z, -107100.0, 0.1);

        std::cout << "\n08NA54: "
                  << "(" << min.x << ", " << min.z << ") - ("
                  << max.x << ", " << max.z << ")" << std::endl;
    }
}