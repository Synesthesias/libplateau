#include <gtest/gtest.h>

#include <citygml/citygml.h>

#include <plateau/geometry/geo_coordinate.h>
#include <plateau/dataset/mesh_code.h>

using namespace citygml;
using namespace plateau::dataset;
using namespace plateau::geometry;

TEST(MeshCode, extentIsProperCoordinate) {
    const std::vector extents = {
        MeshCode("53394525").getExtent(),
        MeshCode("5339353714").getExtent()
    };

    const std::vector expects = {
        GeoCoordinate(35.68731814, 139.68926804, 0),
        GeoCoordinate(35.61065037, 139.71572451, 0)
    };

    for (size_t i = 0; i < extents.size(); ++i) {
        const auto extent = extents[i];
        const auto expected = expects[i];

        ASSERT_GE(extent.max.latitude, expected.latitude);
        ASSERT_GE(extent.max.longitude, expected.longitude);
        ASSERT_LE(extent.min.latitude, expected.latitude);
        ASSERT_LE(extent.min.longitude, expected.longitude);
    }
}

TEST(MeshCode, extentIsProperSize) {
    const auto extent3 = MeshCode("53394525").getExtent();
    const auto extent4 = MeshCode("533945251").getExtent();
    const auto extent5 = MeshCode("5339452513").getExtent();

    ASSERT_LE(abs(extent3.max.longitude - extent3.min.longitude - 1.0 / 8.0 / 10.0), 0.0001);
    ASSERT_LE(abs(extent3.max.latitude - extent3.min.latitude - 2.0 / 3.0 / 8.0 / 10.0), 0.0001);
    ASSERT_LE(abs(extent4.max.longitude - extent4.min.longitude - 1.0 / 8.0 / 10.0 / 2.0), 0.0001);
    ASSERT_LE(abs(extent4.max.latitude - extent4.min.latitude - 2.0 / 3.0 / 8.0 / 10.0 / 2.0), 0.0001);
    ASSERT_LE(abs(extent5.max.longitude - extent5.min.longitude - 1.0 / 8.0 / 10.0 / 4.0), 0.0001);
    ASSERT_LE(abs(extent5.max.latitude - extent5.min.latitude - 2.0 / 3.0 / 8.0 / 10.0 / 4.0), 0.0001);
}

TEST(MeshCode, getMeshCodeByPoint) {
    const auto point = GeoCoordinate(31.88443855, 130.87610481, 0);
    const Extent extent(point, point);
    std::vector<MeshCode> mesh_codes;
    MeshCode::getThirdMeshes(extent, mesh_codes);

    ASSERT_EQ(mesh_codes.size(), 1);
    ASSERT_STREQ(mesh_codes[0].get().c_str(), "47306760");
}

TEST(MeshCode, getMeshCodesByExtent) {
    const Extent extent(
        // 48307698
        GeoCoordinate(32.66449582, 130.85210642, 0),
        // 49300721
        GeoCoordinate(32.68385837, 130.89797434, 0)
    );
    std::vector<MeshCode> mesh_codes;
    MeshCode::getThirdMeshes(extent, mesh_codes);
    for (const auto& mesh_code : mesh_codes) {
        std::cout << mesh_code.get() << std::endl;
    }

    ASSERT_EQ(mesh_codes.size(), 16);
}
