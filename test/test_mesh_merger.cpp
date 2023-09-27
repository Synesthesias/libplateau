#include "gtest/gtest.h"
#include <plateau/polygon_mesh/mesh_factory.h>
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
        params_.keepVertices = true;
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
    auto mesh_1 = model->getRootNodeAt(0).getChildAt(0).getMesh();
    auto mesh_2 = model->getRootNodeAt(0).getChildAt(1).getMesh();
    auto vert_count_1 = mesh_1->getVertices().size();
    auto indices_count_1 = mesh_1->getIndices().size();
    auto uv_1_count_1 = mesh_1->getUV1().size();
    auto uv_4_count_1 = mesh_1->getUV4().size();
    auto vert_count_2 = mesh_2->getVertices().size();
    auto indices_count_2 = mesh_2->getIndices().size();
    auto uv_1_count_2 = mesh_2->getUV1().size();
    auto uv_4_count_2 = mesh_2->getUV4().size();
    ASSERT_GT(vert_count_1, 0);
    ASSERT_GT(indices_count_1, 0);
    ASSERT_GT(uv_1_count_1, 0);
    ASSERT_GT(uv_4_count_1, 0);
    ASSERT_GT(vert_count_2, 0);
    ASSERT_GT(indices_count_2, 0);
    ASSERT_GT(uv_1_count_2, 0);
    ASSERT_GT(uv_4_count_2, 0);
    MeshMerger::mergeMesh(*mesh_1, *mesh_2, false, true);
    ASSERT_EQ(vert_count_1 + vert_count_2, mesh_1->getVertices().size());
    ASSERT_EQ(indices_count_1 + indices_count_2, mesh_1->getIndices().size());
    ASSERT_EQ(uv_1_count_1 + uv_1_count_2, mesh_1->getUV1().size());
    ASSERT_EQ(uv_4_count_1 + uv_4_count_2, mesh_1->getUV4().size());
}


TEST_F(MeshMergerTest, uv4_value_is_the_same_for_the_same_feature) {
    const auto model = MeshExtractor::extract(*city_model_, mesh_extract_options_);
    const auto mesh1 = model->getRootNodeAt(0).getChildAt(0).getMesh();
    const auto uv4 = mesh1->getUV4();
    auto city_object_list = mesh1->getCityObjectList();
    std::map<std::string, int> uv_sizes;
    for (const auto& vt: uv4)
    {
        const auto city_object_index = CityObjectIndex::fromUV(vt);
        const auto gml_id = city_object_list.getAtomicGmlID(city_object_index);
        if (uv_sizes.find(gml_id) != uv_sizes.end())
            ++uv_sizes[gml_id];
        else
            uv_sizes[gml_id] = 1;
    }

    for (const auto& [gml_id, uv_size] : uv_sizes) {
        const auto expected = city_model_->getCityObjectById(gml_id)->getGeometry(0).getPolygon(0)->getVertices().size();
        const auto actual = uv_size;
        ASSERT_EQ(expected, actual);
    }
}


TEST_F(MeshMergerTest, mesh_add_sub_mesh) {
    std::vector<TVec3d> vertices = { TVec3d(11, 12, 13),
                                    TVec3d(21, 22, 23),
                                    TVec3d(31, 32, 33) };
    std::vector<unsigned int> indices = { 0, 1, 2 };
    std::vector<TVec2f> uv_1 = { TVec2f(0.11, 0.12),
                                TVec2f(0.21, 0.22),
                                TVec2f(0.31, 0.32) };
    std::vector<TVec2f> uv_4 = {{0,0}, {0,0}, {0,0}};
    std::vector<SubMesh> sub_meshes = {
            SubMesh(0, 2, "", nullptr)
    };

    auto mesh = Mesh(std::move(vertices), std::move(indices), std::move(uv_1), std::move(uv_4), std::move(sub_meshes), CityObjectList());

    mesh.addSubMesh("test.png", nullptr, 3, 5);

    const auto sub_mesh = mesh.getSubMeshes().at(1);
    ASSERT_EQ("test.png", sub_mesh.getTexturePath());
    ASSERT_EQ(3, sub_mesh.getStartIndex());
    ASSERT_EQ(5, sub_mesh.getEndIndex());
}


TEST_F(MeshMergerTest, mesh_merger_merge) {
    std::vector<TVec3d> vertices = {TVec3d(11, 12, 13),
                                    TVec3d(21, 22, 23),
                                    TVec3d(31, 32, 33)};
    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<TVec2f> uv_1 = {TVec2f(0.11, 0.12),
                                TVec2f(0.21, 0.22),
                                TVec2f(0.31, 0.32)};
    std::vector<TVec2f> uv_4 = {{0,0}, {0,0}, {0,0}};
    std::vector<SubMesh> sub_meshes = {
            SubMesh(0, 2, "", nullptr)
    };

    std::vector<TVec3d> vertices_to_add = { TVec3d(14, 15, 16),
                                TVec3d(24, 25, 26),
                                TVec3d(34, 35, 36) };
    std::vector<unsigned int> indices_to_add = { 0, 1, 2 };
    std::vector<TVec2f> uv_1_to_add = { TVec2f(0.11, 0.12),
                                TVec2f(0.21, 0.22),
                                TVec2f(0.31, 0.32) };
    std::vector<TVec2f> uv_4_to_add = {{1,0}, {1,0}, {1,0} };
    std::vector<SubMesh> sub_meshes_to_add = {
            SubMesh(0, 2, "", nullptr)
    };

    auto mesh = Mesh(std::move(vertices), std::move(indices), std::move(uv_1), std::move(uv_4), std::move(sub_meshes), CityObjectList());

    auto mesh_to_add = Mesh(std::move(vertices_to_add), std::move(indices_to_add), std::move(uv_1_to_add), std::move(uv_4_to_add), std::move(sub_meshes_to_add), CityObjectList());

    MeshMerger::mergeMesh(mesh, mesh_to_add, false, true);
    ASSERT_EQ(33, mesh.getVertices().at(2).z);
    ASSERT_EQ(2, mesh.getIndices().at(2));
    ASSERT_EQ(static_cast<float>(0.32), mesh.getUV1().at(2).y);
    ASSERT_EQ(36, mesh.getVertices().at(5).z);
    ASSERT_EQ(5, mesh.getIndices().at(5));
    ASSERT_EQ(static_cast<float>(0.32), mesh.getUV1().at(5).y);

}
