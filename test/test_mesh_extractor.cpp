#include "gtest/gtest.h"
#include "citygml/citymodel.h"
#include "citygml/citygml.h"
#include "../src/c_wrapper/mesh_extractor_c.cpp"
#include "../src/c_wrapper/model_c.cpp"
#include "../src/c_wrapper/city_model_c.cpp"
#include "../src/c_wrapper/citygml_c.cpp"
#include "plateau/geometry/grid_merger.h"
#include <plateau/geometry/mesh_extractor.h>

using namespace citygml;
using namespace plateau::geometry;

class MeshExtractorTest : public ::testing::Test {
protected:
    void SetUp() override {
        params_.tesselate = true;
    }

    // テストで使う共通パーツです。
    MeshExtractor mesh_extractor_ = MeshExtractor();
    ParserParams params_;
    const std::string gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";
    MeshExtractOptions mesh_extract_options_ = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5);;
    std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
    void test_extract_from_c_wrapper();
};


TEST_F(MeshExtractorTest, extract_returns_model_with_child_with_name){
    auto model = mesh_extractor_.extract(*city_model_, mesh_extract_options_);
    auto nodes = model->getNodesRecursive();
    auto& root_node = model->getRootNodeAt(0);
    std::string root_name = root_node.getName();
    ASSERT_EQ(root_name, "ModelRoot");

    const auto& child_node = root_node.getChildAt(0);
    auto& child_name = child_node.getName();
    ASSERT_EQ(child_name, "grid0");


}

TEST_F(MeshExtractorTest, extract_result_have_texture_url){
    auto model = mesh_extractor_.extract(*city_model_, mesh_extract_options_);
    auto nodes = model->getNodesRecursive();
    auto& root_node = model->getRootNodeAt(0);
    // 存在するテクスチャURLを検索します。
    int num_child = root_node.getChildCount();
    int found_texture_num = 0;
    for(int i=0; i < num_child; i++){
        auto child = root_node.getChildAt(i);
        auto mesh_opt = child.getMesh();
        if(!mesh_opt.has_value()) continue;
        auto& multi_tex = mesh_opt.value().getMultiTexture();
        if(multi_tex.empty()) continue;
        for(auto& texPair : multi_tex){
            auto& texUrl = texPair.second;
            if(texUrl.empty()) continue;
            found_texture_num++;
        }
    }
    ASSERT_TRUE(found_texture_num > 5);
}

TEST_F(MeshExtractorTest, extract_can_exec_multiple_times){
    for(int i=0; i<3; i++){
        test_extract_from_c_wrapper();
    }
}


void MeshExtractorTest::test_extract_from_c_wrapper(){

    const CityModelHandle* city_model_handle;
    plateau_load_citygml(gml_path_.c_str(), plateau_citygml_parser_params(), &city_model_handle, DllLogLevel::LL_WARNING, nullptr, nullptr, nullptr);
    MeshExtractor* mesh_extractor;
    plateau_mesh_extractor_new(&mesh_extractor);

    Model* model;
    plateau_mesh_extractor_extract(mesh_extractor, city_model_handle, mesh_extract_options_, &model);
    auto nodes = model->getNodesRecursive();

    ASSERT_TRUE(model->getRootNodesCount() == 1);
    ASSERT_EQ(model->getRootNodeAt(0).getChildAt(0).getName(), "grid0");
    plateau_model_delete(model);
    plateau_mesh_extractor_delete(mesh_extractor);
}