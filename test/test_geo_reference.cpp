#include <gtest/gtest.h>
#include <plateau/geometry/geo_coordinate.h>
#include <plateau/geometry/geo_reference.h>
#include "../src/geometry/polar_to_plane_cartesian.cpp"

namespace plateau::geometry {
    class GeoReferenceTest : public ::testing::Test {
    protected:
        void SetUp() override {
        }
        void TearDown() override {
        }

        //　テスト設定
        int zone_id = 9;
        TVec3d ref_point = TVec3d(0, 0, 0);
        float unit_scale = 1.0;
        CoordinateSystem coordinate = CoordinateSystem::EUN;
        GeoReference ref = GeoReference(zone_id, ref_point, unit_scale, coordinate);
        TVec3d base_point = TVec3d(100, 100, 0);
    };

    TEST_F(GeoReferenceTest, ConvertAxisPolar) { // NOLINT
        // 平面直角座標変換・座標軸変換を行う
        TVec3d converted = ref.convert(base_point, true, 6697);
        TVec3d projected = ref.project(base_point);

        // Expected
        TVec3d position = base_point;
        PolarToPlaneCartesian().project(position, zone_id);
        TVec3 expected_point = GeoReference::convertAxisFromENUTo(coordinate, position);
        expected_point = expected_point / unit_scale - ref_point;

        ASSERT_EQ(expected_point, converted);
        ASSERT_EQ(expected_point, projected);
    }

    TEST_F(GeoReferenceTest, ConvertProjectPolar) { // NOLINT
        // 平面直角座標変換を行う・座標軸変換を行わない
        TVec3d converted = ref.convert(base_point, false, 6697);
        TVec3d projected = ref.projectWithoutAxisConvert(base_point);

        // Expected
        TVec3d position = base_point;
        PolarToPlaneCartesian().project(position, zone_id);
        TVec3 expected_point = position / unit_scale - GeoReference::convertAxisToENU(coordinate, ref_point);

        ASSERT_EQ(expected_point, converted);
        ASSERT_EQ(expected_point, projected);
    }

    TEST_F(GeoReferenceTest, ConvertAxisPlane) { // NOLINT
        // 平面->緯度経度->平面変換・座標軸変換を行う
        TVec3d point = ref.convert(base_point, true, 10169);

        // Expected
        TVec3d position = base_point;
        const auto& unprj = GeoReference::planeToPolar(position, CoordinateReferenceFactory::GetZoneId(10169));
        position = { unprj.latitude, unprj.longitude, unprj.height };
        PolarToPlaneCartesian().project(position, zone_id);
        TVec3 expected_point = GeoReference::convertAxisFromENUTo(coordinate, position);
        expected_point = expected_point / unit_scale - ref_point;

        ASSERT_EQ(expected_point, point);
    }

    TEST_F(GeoReferenceTest, ConvertPlane) { // NOLINT
        // 平面->緯度経度->平面変換を行う・座標軸変換を行わない
        TVec3d point = ref.convert(base_point, false, 10169);

        // Expected
        TVec3d position = base_point;
        const auto& unprj = GeoReference::planeToPolar(position, CoordinateReferenceFactory::GetZoneId(10169));
        position = { unprj.latitude, unprj.longitude, unprj.height };
        PolarToPlaneCartesian().project(position, zone_id);
        TVec3 expected_point = position / unit_scale - GeoReference::convertAxisToENU(coordinate, ref_point);

        ASSERT_EQ(expected_point, point);
    }

    // 平面直角座標のGMLの値を緯度経度に変換
    TEST_F(GeoReferenceTest, PlaneToPolarConversion) { // NOLINT       
        TVec3d base_position(100, -100, 1); // 経度、緯度、高さ

        const auto& polar = GeoReference::planeToPolar(base_position, 9);
        TVec3d polar_vector = { polar.latitude, polar.longitude, polar.height };

        // xy反転してunprojectした値
        TVec3d position = { base_position.y, base_position.x, base_position.z }; // xy反転
        PolarToPlaneCartesian().unproject(position, 9);
        const auto& expected_point = position;

        ASSERT_EQ(expected_point, polar_vector);
    }

}
