#include "gtest/gtest.h"
#include <plateau/polygon_mesh/mesh_merger.h>
#include "citygml/citygml.h"
#include "plateau/polygon_mesh/mesh_extractor.h"
#include "../src/c_wrapper/mesh_merger_c.cpp"


using namespace citygml;
using namespace plateau::polygonMesh;
using namespace plateau::geometry;

class MeshMergerTest : public ::testing::Test {
protected:
    void SetUp() override {
        params_.tesselate = true;
        mesh_extract_options_.min_lod = 2;
        mesh_extract_options_.max_lod = 2;
        mesh_extract_options_.mesh_granularity = MeshGranularity::PerCityModelArea;
        mesh_extract_options_.grid_count_of_side = 5;
    }

    ParserParams params_;
    const std::string gml_path_ = "../data/日本語パステスト/udx/bldg/53392642_bldg_6697_op2.gml";
    MeshExtractOptions mesh_extract_options_ = MeshExtractOptions();
    const std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
};

TEST_F(MeshMergerTest, when_merge_then_vertices_count_is_sum_of_before) {
    auto model = MeshExtractor::extract(*city_model_, mesh_extract_options_);
    auto& mesh_1 = model->getRootNodeAt(0).getChildAt(0).getMesh();
    auto& mesh_2 = model->getRootNodeAt(0).getChildAt(1).getMesh();
    auto vert_count_1 = mesh_1->getVertices().size();
    auto indices_count_1 = mesh_1->getIndices().size();
    auto uv_1_count_1 = mesh_1->getUV1().size();
    auto uv_2_count_1 = mesh_1->getUV2().size();
    auto uv_3_count_1 = mesh_1->getUV3().size();
    auto vert_count_2 = mesh_2->getVertices().size();
    auto indices_count_2 = mesh_2->getIndices().size();
    auto uv_1_count_2 = mesh_2->getUV1().size();
    auto uv_2_count_2 = mesh_2->getUV2().size();
    auto uv_3_count_2 = mesh_2->getUV3().size();
    ASSERT_GT(vert_count_1, 0);
    ASSERT_GT(indices_count_1, 0);
    ASSERT_GT(uv_1_count_1, 0);
    ASSERT_GT(uv_2_count_1, 0);
    ASSERT_GT(uv_3_count_1, 0);
    ASSERT_GT(vert_count_2, 0);
    ASSERT_GT(indices_count_2, 0);
    ASSERT_GT(uv_1_count_2, 0);
    ASSERT_GT(uv_2_count_2, 0);
    ASSERT_GT(uv_3_count_2, 0);
    MeshMerger::mergeMesh(*mesh_1, *mesh_2, false, true,
                          TVec2f{0, 0}, TVec2f{0, 0});
    ASSERT_EQ(vert_count_1 + vert_count_2, mesh_1->getVertices().size());
    ASSERT_EQ(indices_count_1 + indices_count_2, mesh_1->getIndices().size());
    ASSERT_EQ(uv_1_count_1 + uv_1_count_2, mesh_1->getUV1().size());
    ASSERT_EQ(uv_2_count_1 + uv_2_count_2, mesh_1->getUV2().size());
    ASSERT_EQ(uv_3_count_1 + uv_3_count_2, mesh_1->getUV3().size());

}

TEST_F(MeshMergerTest, mesh_merger_info) {
    std::vector<TVec3d> vertices = {TVec3d(11, 12, 13),
                                    TVec3d(21, 22, 23),
                                    TVec3d(31, 32, 33)};
    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<TVec2f> uv_1 = {TVec2f(0.11, 0.12),
                                TVec2f(0.21, 0.22),
                                TVec2f(0.31, 0.32)};
    std::vector<SubMesh> sub_meshes = {
            SubMesh(0, 2, "")
    };
    auto mesh = Mesh();
    // ここでの CoordinateSystem の設定によっては Indices の順番が逆転するので注意してください。
    MeshMerger::mergeMeshInfo(mesh, std::move(vertices), std::move(indices), std::move(uv_1),
                              std::move(sub_meshes), CoordinateSystem::ENU, CoordinateSystem::ENU, true);
    ASSERT_EQ(33, mesh.getVertices().at(2).z);
    ASSERT_EQ(2, mesh.getIndices().at(2));
    ASSERT_EQ((float) 0.32, mesh.getUV1().at(2).y);

}
