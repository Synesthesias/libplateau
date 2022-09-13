#include <gtest/gtest.h>

#include <citygml/citygml.h>

#include <plateau/geometry/geo_coordinate.h>
#include <plateau/udx/mesh_code.h>

using namespace citygml;
using namespace plateau::udx;
using namespace plateau::geometry;

TEST(MeshCode, extentIsProperCoordinate) {
    const auto extent = MeshCode("53394525").getExtent();
    const GeoCoordinate expected(35.68731814, 139.68926804, 0);

    ASSERT_GE(extent.max.latitude, expected.latitude);
    ASSERT_GE(extent.max.longitude, expected.longitude);
    ASSERT_LE(extent.min.latitude, expected.latitude);
    ASSERT_LE(extent.min.longitude, expected.longitude);
}

TEST(MeshCode, extentIsProperSize) {
    const auto extent = MeshCode("53394525").getExtent();

    ASSERT_LE(abs(extent.max.longitude - extent.min.longitude - 1.0 / 8.0 / 10.0), 0.001);
    ASSERT_LE(abs(extent.max.latitude - extent.min.latitude - 2.0 / 3.0 / 8.0 / 10.0), 0.001);
}
//
//TEST(MeshCode, getMeshCodeByPoint) {
//    const auto point = GeoCoordinate(31.88443855, 130.87610481, 0);
//    Extent extent;
//    extent.min = point;
//    extent.max = point;
//    std::vector<MeshCode> mesh_codes;
//    MeshCode::getThirdMeshes(extent, mesh_codes);
//
//    ASSERT_EQ(mesh_codes.size(), 1);
//    ASSERT_STREQ(mesh_codes[0].get().c_str(), "47306760");
//}
//
//TEST(MeshCode, getMeshCodesByExtent) {
//    Extent extent;
//    // 48307698
//    extent.min = GeoCoordinate(32.66449582, 130.85210642, 0);
//    // 49300721
//    extent.max = GeoCoordinate(32.68385837, 130.89797434, 0);
//    std::vector<MeshCode> mesh_codes;
//    MeshCode::getThirdMeshes(extent, mesh_codes);
//    for (const auto& mesh_code : mesh_codes) {
//        std::cout << mesh_code.get() << std::endl;
//    }
//
//    ASSERT_EQ(mesh_codes.size(), 16);
//}
