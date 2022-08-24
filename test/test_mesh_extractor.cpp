#include "gtest/gtest.h"
#include "citygml/citymodel.h"
#include "citygml/citygml.h"
#include "citygml/polygon.h"
#include "../src/c_wrapper/mesh_extractor_c.cpp"
#include "../src/c_wrapper/model_c.cpp"
#include "../src/c_wrapper/city_model_c.cpp"
#include "../src/c_wrapper/citygml_c.cpp"
#include <plateau/geometry/mesh_extractor.h>

using namespace citygml;
using namespace plateau::geometry;

class MeshExtractorTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";

        params_.tesselate = true;
    }
    ParserParams params_;
    std::string gml_path_;
};

TEST_F(MeshExtractorTest, gridMerge_returns_polygons_with_vertices){
    auto meshExtractor = MeshExtractor();
    auto options = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5);

    auto cityModel = load(gml_path_, params_);
    auto result = meshExtractor.gridMerge(*cityModel, options);
    int numPolyWithVert = 0;
    for(auto& poly : result){
        if(!poly.getVertices().empty()){
            numPolyWithVert++;
        }
    }
    ASSERT_TRUE(numPolyWithVert >= 5);
}

TEST_F(MeshExtractorTest, gridMerge_uv1_size_matches_num_of_vertices){
    auto meshExtractor = MeshExtractor();
    auto options = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5);
    auto cityModel = load(gml_path_, params_);
    auto result = meshExtractor.gridMerge(*cityModel, options);
    for(auto& poly : result){
        auto sizeOfUV1 = poly.getUV1().size();
        auto numOfVertices = poly.getVertices().size();
        ASSERT_EQ(sizeOfUV1, numOfVertices);
    }
}

TEST_F(MeshExtractorTest, extract_returns_model_with_child_and_name){
    auto meshExtractor = MeshExtractor();
    auto options = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5);
    auto cityModel = load(gml_path_, params_);
    auto model = meshExtractor.extract(*cityModel, options);
    auto nodes = model->getNodesRecursive();

    // debug print
//    for(auto node : nodes){
//        auto& meshOpt = node->getMesh();
//        std::string meshName = meshOpt ? meshOpt->getId() : "noneMesh";
//        auto verticesCount = meshOpt ? meshOpt->getVertices().size() : 0;
//        std::cout << node->getName() << " : meshId=" << meshName << ", verticesCount=" << verticesCount <<  std::endl;
//    }

    std::string rootName = model->getRootNodeAt(0).getName();
    ASSERT_EQ(rootName, "ModelRoot");
    auto rootNode = model->getRootNodeAt(0);
    auto childNode = rootNode.getChildAt(0);
    std::string firstChildName = childNode.getName();
    ASSERT_EQ(firstChildName, "grid0");

    // テクスチャURLがあることの確認
    int numChild = rootNode.getChildCount();
    int foundTextureNum = 0;
    for(int i=0; i<numChild; i++){
        auto child = rootNode.getChildAt(i);
        auto meshOpt = child.getMesh();
        if(!meshOpt.has_value()) continue;
        auto& multiTex = meshOpt.value().getMultiTexture();
        if(multiTex.empty()) continue;
        for(auto texPair : multiTex){
            auto& texUrl = texPair.second;
            if(texUrl.empty()) continue;
            std::cout << "texture url: " << texUrl << std::endl;
            foundTextureNum++;
        }
    }
    ASSERT_TRUE(foundTextureNum > 0);
}

// TODO 整理したい
void test_extract_from_c_wrapper(){
    auto gml_path = "../data/udx/bldg/53392642_bldg_6697_op2.gml";

    ParserParams params;
    params.tesselate = true;

//    auto city_model = load(gml_path, params);
    const CityModelHandle* cityModelHandleConst;
    plateau_load_citygml(gml_path, plateau_citygml_parser_params(), &cityModelHandleConst, DllLogLevel::LL_INFO, nullptr, nullptr, nullptr);
    MeshExtractor* meshExtractor;
    plateau_mesh_extractor_new(&meshExtractor);
    auto options = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5);

    Model* model;
    CityModelHandle* cityModelHandle = const_cast<CityModelHandle *>(cityModelHandleConst);
    plateau_mesh_extractor_extract(meshExtractor, cityModelHandle, options, &model);
//    auto model = meshExtractor->extract_to_row_pointer(*city_model, options, logger);
    auto nodes = model->getNodesRecursive();

    ASSERT_TRUE(model->getRootNodesCount() == 1);
    ASSERT_EQ(model->getRootNodeAt(0).getChildAt(0).getName(), "grid0");
    plateau_model_delete(model);
    plateau_mesh_extractor_delete(meshExtractor);
}

// Unityで数回ロードすると落ちるバグを再現しようとしたもの
TEST_F(MeshExtractorTest, extract_can_exec_multiple_times){
    for(int i=0; i<3; i++){
        test_extract_from_c_wrapper();
    }
}