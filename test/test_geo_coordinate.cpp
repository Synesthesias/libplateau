#include <gtest/gtest.h>
#include "../src/geometry/geo_coordinate.cpp"

namespace plateau::geometry {
    class GeoCoordinateTest : public ::testing::Test {
    protected:
        void SetUp() override {
        }
        void TearDown() override {
        }
    };

    TEST_F(GeoCoordinateTest, ExtentContains) { // NOLINT

        plateau::geometry::Extent ext(GeoCoordinate(-30, -90, -9999), GeoCoordinate(30, 90, 9999));
        plateau::geometry::Extent ext2(GeoCoordinate(-30, -90, -9999), GeoCoordinate(60, 180, 9999));

        ASSERT_TRUE(ext.contains(TVec3d(0, 0, 0)));
        ASSERT_TRUE(ext.contains(GeoCoordinate(1,1,0)));
        ASSERT_FALSE(ext.contains(GeoCoordinate(40, 0, 0)));
        ASSERT_FALSE(ext.contains(GeoCoordinate(0, -100, 0)));

        const auto& zoneRefPoint = GeoReference::planeToPolar(TVec3d(), 8);// 10169 は zone id:8
        ASSERT_FLOAT_EQ(36.0, zoneRefPoint.latitude);
        ASSERT_FLOAT_EQ(138.5, zoneRefPoint.longitude);
        ASSERT_FALSE(ext.containsInPolar(TVec3d(), 10169));
        ASSERT_TRUE(ext2.containsInPolar(TVec3d(), 10169));      
    }

    TEST_F(GeoCoordinateTest, CoordinateReference) { // NOLINT

        ASSERT_EQ(8, CoordinateReferenceFactory::GetZoneId(10169));
        ASSERT_FALSE(CoordinateReferenceFactory::IsPolarCoordinateSystem(10169));
        ASSERT_TRUE(CoordinateReferenceFactory::IsPolarCoordinateSystem(6697));

        const auto& refPoint = CoordinateReferenceFactory::GetReferencePoint(10169);
        ASSERT_EQ(36.0, refPoint.latitude);
        ASSERT_EQ(138.5, refPoint.longitude);
    }
}
