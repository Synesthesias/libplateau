#include "gtest/gtest.h"
#include "plateau/geometry/grid_merger.h"
#include "citygml/citygml.h"

using namespace citygml;
using namespace plateau::geometry;

class GridMergerTest : public ::testing::Test {
protected:
    void SetUp() override {
        params_.tesselate = true;
    }

    ParserParams params_;
    const std::string gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";
    MeshExtractOptions mesh_extract_options_ = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5, 1.0);
    std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
};

TEST_F(GridMergerTest, gridMerge_returns_meshes_with_vertices){ // NOLINT
    auto result = GridMerger::gridMerge(*city_model_, mesh_extract_options_);
    int num_mesh_with_vert = 0;
    for(auto& mesh : result){
        if(!mesh.getVertices().empty()){
            num_mesh_with_vert++;
        }
    }
    ASSERT_TRUE(num_mesh_with_vert >= 5);
}

TEST_F(GridMergerTest, gridMerge_uv_size_matches_num_of_vertices){ // NOLINT
    auto result = GridMerger::gridMerge(*city_model_, mesh_extract_options_);
    for(auto& mesh : result){
        auto size_of_uv1 = mesh.getUV1().size();
        auto size_of_uv2 = mesh.getUV2().size();
        auto size_of_uv3 = mesh.getUV3().size();
        auto num_of_vertices = mesh.getVertices().size();
        ASSERT_EQ(size_of_uv1, num_of_vertices);
        ASSERT_EQ(size_of_uv2, num_of_vertices);
        ASSERT_EQ(size_of_uv3, num_of_vertices);
    }
}