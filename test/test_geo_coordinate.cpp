#include <gtest/gtest.h>
#include "../src/geometry/geo_coordinate.cpp"

namespace plateau::geometry {
    class GeoCoordinateTest : public ::testing::Test {
    protected:
    };

    TEST_F(GeoCoordinateTest, ExtentContains) { // NOLINT

        plateau::geometry::Extent ext(GeoCoordinate(-30, -90, -9999), GeoCoordinate(30, 90, 9999));
        plateau::geometry::Extent ext2(GeoCoordinate(-30, -90, -9999), GeoCoordinate(60, 180, 9999));

        ASSERT_TRUE(ext.contains(TVec3d(0, 0, 0)));
        ASSERT_TRUE(ext.contains(GeoCoordinate(1,1,0)));
        ASSERT_FALSE(ext.contains(GeoCoordinate(40, 0, 0)));
        ASSERT_FALSE(ext.contains(GeoCoordinate(0, -100, 0)));

        // 極座標系EPSG:6697（日本測地系2000）の場合、containsとcontainsInPolarの結果が一致することを確認
        ASSERT_EQ(ext.contains(TVec3d(0, 0, 0)), ext.containsInPolar(TVec3d(0, 0, 0), 6697));
        ASSERT_EQ(ext.contains(TVec3d(80, 90, 0)), ext.containsInPolar(TVec3d(80, 90, 0), 6697));

        // 平面直角座標
        // EPSG:10169は平面直角座標系（第8系）
        const auto& zoneRefPoint = GeoReference::planeToPolar(TVec3d(), 8);
        ASSERT_FLOAT_EQ(36.0, zoneRefPoint.latitude);
        ASSERT_FLOAT_EQ(138.5, zoneRefPoint.longitude);
        ASSERT_FALSE(ext.containsInPolar(TVec3d(), 10169));
        ASSERT_TRUE(ext2.containsInPolar(TVec3d(), 10169));      
    }

    TEST_F(GeoCoordinateTest, CoordinateReference) { // NOLINT

        // 正常系テスト - EPSGから正しいゾーンIDが取得できること
        ASSERT_EQ(1, CoordinateReferenceFactory::GetZoneId(10162)); // 最小値
        ASSERT_EQ(8, CoordinateReferenceFactory::GetZoneId(10169)); // 中間値
        ASSERT_EQ(13, CoordinateReferenceFactory::GetZoneId(10174)); // 最大値
        
        // 異常系テスト - 範囲外のEPSGでは0が返されること
        ASSERT_EQ(0, CoordinateReferenceFactory::GetZoneId(10161)); // 境界外（最小値-1）
        ASSERT_EQ(0, CoordinateReferenceFactory::GetZoneId(10175)); // 境界外（最大値+1）
        ASSERT_EQ(0, CoordinateReferenceFactory::GetZoneId(6697)); // 極座標系EPSG
        
        ASSERT_FALSE(CoordinateReferenceFactory::IsPolarCoordinateSystem(10169));
        ASSERT_TRUE(CoordinateReferenceFactory::IsPolarCoordinateSystem(6697));
        ASSERT_TRUE(CoordinateReferenceFactory::IsPolarCoordinateSystem(4301)); // JGD2000
        ASSERT_TRUE(CoordinateReferenceFactory::IsPolarCoordinateSystem(0)); // 不明なEPSG

        const auto& refPoint = CoordinateReferenceFactory::GetOriginPoint(10169);
        ASSERT_FLOAT_EQ(36.0, refPoint.latitude);
        ASSERT_FLOAT_EQ(138.5, refPoint.longitude);
        
        // 無効なEPSGの場合、空のGeoCoordinateが返されること
        const auto & invalidRefPoint = CoordinateReferenceFactory::GetOriginPoint(0);
        ASSERT_EQ(0.0, invalidRefPoint.latitude);
        ASSERT_EQ(0.0, invalidRefPoint.longitude);
        ASSERT_EQ(0.0, invalidRefPoint.height);
    }
}
