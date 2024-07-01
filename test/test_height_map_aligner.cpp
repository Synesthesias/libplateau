#include "gtest/gtest.h"
#include "citygml/citygml.h"
#include "plateau/height_map_alighner/height_map_aligner.h"

using namespace plateau::heightMapAligner;

class HeightMapAlignerTest : public ::testing::Test {

};

TEST_F(HeightMapAlignerTest, on_zero_heightmap) {
    auto aligner = HeightMapAligner(0);
    auto model = plateau::polygonMesh::Model();
    auto frame = HeightMapFrame(std::vector<uint16_t>(), 0, 0, 0, 0, 0, 0, 0, 0);
    aligner.addHeightmapFrame(frame);
    aligner.align(model, 4);
}
