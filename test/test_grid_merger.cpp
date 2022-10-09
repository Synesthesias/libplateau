#include "gtest/gtest.h"
#include "citygml/citygml.h"
#include "../src/polygon_mesh/grid_merger.h"

using namespace citygml;
using namespace plateau::polygonMesh;

class GridMergerTest : public ::testing::Test {
protected:
    void SetUp() override {
        params_.tesselate = true;
    }

    ParserParams params_;
    const std::string gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";
    const MeshExtractOptions mesh_extract_options_ = MeshExtractOptions(TVec3d(0, 0, 0), CoordinateSystem::WUN,
                                                                        MeshGranularity::PerCityModelArea, 2, 2, true,
                                                                        5, 1.0, 9, true, false,
                                                                        Extent(GeoCoordinate(-90, -180, -999), GeoCoordinate(90,180,999)));
    std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
    const GeoReference geo_reference_ = GeoReference(mesh_extract_options_.coordinate_zone_id, mesh_extract_options_.reference_point,
                                                     mesh_extract_options_.unit_scale, mesh_extract_options_.mesh_axes);
};

TEST_F(GridMergerTest, gridMerge_returns_meshes_with_vertices) { // NOLINT
    auto result = GridMerger::gridMerge(*city_model_, mesh_extract_options_, 0, geo_reference_);
    int num_mesh_with_vert = 0;
    for (const auto& [id, mesh]: result) {
        if (!mesh.getVertices().empty()) {
            num_mesh_with_vert++;
        }
    }
    ASSERT_TRUE(num_mesh_with_vert >= 5);
}

TEST_F(GridMergerTest, gridMerge_uv_size_matches_num_of_vertices) { // NOLINT
    auto result = GridMerger::gridMerge(*city_model_, mesh_extract_options_, 0, geo_reference_);
    for (const auto& [id, mesh]: result) {
        auto size_of_uv1 = mesh.getUV1().size();
        auto size_of_uv2 = mesh.getUV2().size();
        auto size_of_uv3 = mesh.getUV3().size();
        auto num_of_vertices = mesh.getVertices().size();
        ASSERT_EQ(size_of_uv1, num_of_vertices);
        ASSERT_EQ(size_of_uv2, num_of_vertices);
        ASSERT_EQ(size_of_uv3, num_of_vertices);
    }
}
