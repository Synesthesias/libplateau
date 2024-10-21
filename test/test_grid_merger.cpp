#include "gtest/gtest.h"
#include "citygml/citygml.h"
#include "../src/polygon_mesh/area_mesh_factory.h"



using namespace citygml;
using namespace plateau::polygonMesh;
using namespace plateau::geometry;

class GridMergerTest : public ::testing::Test {
protected:
    void SetUp() override {
        params_.tesselate = true;
    }

    ParserParams params_;
    const std::string gml_path_ = "../data/日本語パステスト/udx/bldg/53392642_bldg_6697_op2.gml";
    std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
    const MeshExtractOptions mesh_extract_options_ = MeshExtractOptions();
    const GeoReference geo_reference_ = GeoReference(mesh_extract_options_.coordinate_zone_id, mesh_extract_options_.reference_point,
                                                     mesh_extract_options_.unit_scale, mesh_extract_options_.mesh_axes);
};

TEST_F(GridMergerTest, gridMerge_returns_meshes_with_vertices) { // NOLINT
    auto result = AreaMeshFactory::gridMerge(*city_model_, mesh_extract_options_, 0, geo_reference_, { Extent::all() });
    int num_mesh_with_vert = 0;
    for (const auto& [id, mesh] : result) {
        if (!mesh->getVertices().empty()) {
            num_mesh_with_vert++;
        }
    }
    ASSERT_TRUE(num_mesh_with_vert >= 5);
}

TEST_F(GridMergerTest, gridMerge_uv_size_matches_num_of_vertices) { // NOLINT
    auto result = AreaMeshFactory::gridMerge(*city_model_, mesh_extract_options_, 0, geo_reference_, { Extent::all() });
    for (const auto& [id, mesh] : result) {
        auto size_of_uv1 = mesh->getUV1().size();
        auto size_of_uv4 = mesh->getUV4().size();
        auto num_of_vertices = mesh->getVertices().size();
        ASSERT_EQ(size_of_uv1, num_of_vertices);
        ASSERT_EQ(size_of_uv4, num_of_vertices);
    }
}
