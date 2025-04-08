#include <gtest/gtest.h>
#include <plateau/geometry/geo_coordinate.h>
#include <plateau/geometry/geo_reference.h>
#include "../src/geometry/polar_to_plane_cartesian.h"

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

    TEST_F(GeoReferenceTest, ConvertAxisProject) { // NOLINT

        // 平面直角座標変換・座標軸変換を行う
        TVec3d converted = ref.convert(base_point, true, true);
        TVec3d projected = ref.project(base_point);

        // Expected
        TVec3d position = base_point;
        PolarToPlaneCartesian().project(position, zone_id);
        TVec3 expected_point = GeoReference::convertAxisFromENUTo(coordinate, position);
        expected_point = expected_point / unit_scale - ref_point;

        ASSERT_EQ(expected_point, converted);
        ASSERT_EQ(expected_point, projected);
    }

    TEST_F(GeoReferenceTest, ConvertProjectOnly) { // NOLINT

        // 平面直角座標変換を行う・座標軸変換を行わない
        TVec3d converted = ref.convert(base_point, false, true);
        TVec3d projected = ref.projectWithoutAxisConvert(base_point);

        // Expected
        TVec3d position = base_point;
        PolarToPlaneCartesian().project(position, zone_id);
        TVec3 expected_point = position / unit_scale - GeoReference::convertAxisToENU(coordinate, ref_point);

        ASSERT_EQ(expected_point, converted);
        ASSERT_EQ(expected_point, projected);
    }

    TEST_F(GeoReferenceTest, ConvertAxisOnly) { // NOLINT

        // 平面直角座標変換を行わない・座標軸変換を行う
        TVec3d point = ref.convert(base_point, true, false);

        // Expected
        TVec3 expected_point = GeoReference::convertAxisFromENUTo(coordinate, base_point);
        expected_point = expected_point / unit_scale - ref_point;

        ASSERT_EQ(expected_point, point);
    }

    TEST_F(GeoReferenceTest, ConvertOnly) { // NOLINT

        // 平面直角座標変換・座標軸変換を行わない
        TVec3d point = ref.convert(base_point, false, false);

        // Expected
        TVec3 expected_point = base_point / unit_scale - GeoReference::convertAxisToENU(coordinate, ref_point);

        ASSERT_EQ(expected_point, point);
    }

    // fetch のテストは test_dataset.cpp にあります。

}
