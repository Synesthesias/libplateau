#include "gtest/gtest.h"
#include "citygml/citymodel.h"
#include "citygml/citygml.h"
#include "../src/c_wrapper/mesh_extractor_c.cpp"
#include "../src/c_wrapper/model_c.cpp"
#include "../src/c_wrapper/city_model_c.cpp"
#include "../src/c_wrapper/citygml_c.cpp"
#include "plateau/geometry/grid_merger.h"
#include <plateau/geometry/mesh_extractor.h>
#include <plateau/geometry/geometry_utils.h>

using namespace citygml;
using namespace plateau::geometry;

class MeshExtractorTest : public ::testing::Test {
protected:
    void SetUp() override {
        params_.tesselate = true;
    }

    // テストで使う共通パーツです。
    ParserParams params_;
    const std::string gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";
    MeshExtractOptions mesh_extract_options_ = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5, 1.0);;
    std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
    void test_extract_from_c_wrapper();
    bool have_vertex_recursive(Node &node);
};


TEST_F(MeshExtractorTest, extract_returns_model_with_child_with_name){ // NOLINT
    auto model = MeshExtractor::extract(*city_model_, mesh_extract_options_);
    auto& root_node = model->getRootNodeAt(0);
    std::string root_name = root_node.getName();
    ASSERT_EQ(root_name, "ModelRoot");

    const auto& first_model_node = root_node.getChildAt(0).getChildAt(0);
    auto& first_model_node_name = first_model_node.getName();
    ASSERT_EQ(first_model_node_name, "grid0");


}

TEST_F(MeshExtractorTest, extract_result_have_texture_url){ // NOLINT
    auto model = MeshExtractor::extract(*city_model_, mesh_extract_options_);
    auto& lod_node = model->getRootNodeAt(0).getChildAt(0);
    // 存在するテクスチャURLを検索します。
    auto num_child = lod_node.getChildCount();
    int found_texture_num = 0;
    for(int i=0; i < num_child; i++){
        auto child = lod_node.getChildAt(i);
        auto mesh_opt = child.getMesh();
        if(!mesh_opt.has_value()) continue;
        auto& sub_meshes = mesh_opt.value().getSubMeshes();
        if(sub_meshes.empty()) continue;
        for(auto& sub_mesh : sub_meshes){
            auto& texUrl = sub_mesh.getTexturePath();
            if(texUrl.empty()) continue;
            found_texture_num++;
        }
    }
    ASSERT_TRUE(found_texture_num > 5);
}

TEST_F(MeshExtractorTest, extract_can_exec_multiple_times){ // NOLINT
    for(int i=0; i<3; i++){
        test_extract_from_c_wrapper();
    }
}

TEST_F(MeshExtractorTest, when_extract_atomic_building_then_primary_nodes_have_no_mesh_and_atomic_nodes_have_mesh){ // NOLINT
    MeshExtractOptions options = mesh_extract_options_;
    options.meshGranularity = MeshGranularity::PerAtomicFeatureObject;
    auto model = MeshExtractor::extract(*city_model_, options);
    auto first_primary_node = model->getRootNodeAt(0).getChildAt(0).getChildAt(0);
    auto first_atomic_node = first_primary_node.getChildAt(0);
    ASSERT_FALSE(first_primary_node.getMesh().has_value());
    ASSERT_TRUE(first_atomic_node.getMesh().has_value());
}

TEST_F(MeshExtractorTest, extract_can_export_multiple_lods_when_granularity_is_per_city_model_area){ // NOLINT
   MeshExtractOptions options = mesh_extract_options_;
   options.meshGranularity = MeshGranularity::PerCityModelArea;
   options.minLOD = 0;
   options.maxLOD = 2;
   auto model = MeshExtractor::extract(*city_model_, options);
   auto root_node = model->getRootNodeAt(0);
   auto lod0 = root_node.getChildAt(0);
   auto lod1 = root_node.getChildAt(1);
   auto lod2 = root_node.getChildAt(2);
   ASSERT_EQ(lod0.getName(), "LOD0" );
   ASSERT_EQ(lod1.getName(), "LOD1");
   ASSERT_EQ(lod2.getName(), "LOD2");
   auto lod0_grid2_num_vertices = lod0.getChildAt(2).getMesh().value().getVerticesConst().size();
   auto lod1_grid2_num_vertices = lod1.getChildAt(2).getMesh().value().getVerticesConst().size();
   auto lod2_grid2_num_vertices = lod2.getChildAt(2).getMesh().value().getVerticesConst().size();
   ASSERT_GT(lod0_grid2_num_vertices, 0);
   ASSERT_GT(lod1_grid2_num_vertices, 0);
   ASSERT_GT(lod2_grid2_num_vertices, 0);
}

TEST_F(MeshExtractorTest, extract_can_export_multiple_lods_with_vertex){ // NOLINT
    std::vector<MeshGranularity> test_pattern_granularity = {MeshGranularity::PerCityModelArea, MeshGranularity::PerPrimaryFeatureObject, MeshGranularity::PerAtomicFeatureObject};
    // 各LODノード以下に頂点が存在することのテストです。
    // メッシュ粒度 と LOD の全ての組み合わせをテストしたいので2重forループを回します。
    for(auto granularity : test_pattern_granularity){
        std::cout << "testing meshGranularity = " << (int)granularity << std::endl;
        MeshExtractOptions options = mesh_extract_options_;
        options.meshGranularity = granularity;
        options.minLOD = 0;
        options.maxLOD = 2;
        auto model = MeshExtractor::extract(*city_model_, options);
        auto root_node = model->getRootNodeAt(0);
        for(int lod=options.minLOD; lod<=options.maxLOD; lod++){
            auto& lod_node = model->getRootNodeAt(0).getChildAt(lod);
            ASSERT_EQ(lod_node.getName(), "LOD"+ std::to_string(lod));
            ASSERT_TRUE(have_vertex_recursive(lod_node));
        }
    }

}


void MeshExtractorTest::test_extract_from_c_wrapper(){

    const CityModelHandle* city_model_handle;
    plateau_load_citygml(gml_path_.c_str(), plateau_citygml_parser_params(), &city_model_handle, DllLogLevel::LL_WARNING, nullptr, nullptr, nullptr);
    MeshExtractor* mesh_extractor;
    plateau_mesh_extractor_new(&mesh_extractor);

    Model* model;
    plateau_mesh_extractor_extract(mesh_extractor, city_model_handle, mesh_extract_options_, &model);

    ASSERT_TRUE(model->getRootNodesCount() == 1);
    ASSERT_EQ(model->getRootNodeAt(0).getChildAt(0).getChildAt(0).getName(), "grid0");
    plateau_model_delete(model);
    plateau_mesh_extractor_delete(mesh_extractor);
}

bool MeshExtractorTest::have_vertex_recursive(Node &node) {
    auto& mesh = node.getMesh();
    if(mesh.has_value() && !(mesh.value().getVerticesConst().empty())){
        return true;
    }
    auto num_child = node.getChildCount();
    for(int i=0; i<num_child; i++){
        if(have_vertex_recursive(node.getChildAt(i))){
            return true;
        }
    }
    return false;
}