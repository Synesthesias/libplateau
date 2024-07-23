#include "gtest/gtest.h"
#include "citygml/citygml.h"
#include <plateau/height_map_generator/heightmap_mesh_generator.h>
#include <plateau/height_map_generator/heightmap_types.h>
#include <memory>
#include <cmath>

using namespace plateau::heightMapGenerator;
using namespace plateau::polygonMesh;
using namespace plateau::geometry;

class HeightMapMeshGeneratorTest : public ::testing::Test {

};

TEST_F(HeightMapMeshGeneratorTest, flat_heightmap_is_converted_to_flat_mesh) { // NOLINT
    auto height_map = HeightMapT{ 0, 0, 0, 0 };

    plateau::heightMapGenerator::HeightmapMeshGenerator generator;
    Mesh mesh;
    generator.generateMeshFromHeightmap(
        mesh,
        2, 2, 1, height_map.data(),
        CoordinateSystem::ENU,
        TVec3d(0, 0, 0), TVec3d(1, 1, 1),
        TVec2f(0, 0), TVec2f(1, 1), true);

    // 実行結果をチェックします
    for (const auto vertex : mesh.getVertices()) {
        ASSERT_EQ(vertex.z, 0);
    }
}
