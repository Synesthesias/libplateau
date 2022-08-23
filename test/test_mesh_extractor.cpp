#include "gtest/gtest.h"
#include "citygml/citymodel.h"
#include "citygml/citygml.h"
#include "citygml/polygon.h"
#include "../src/c_wrapper/mesh_extractor_c.cpp"
#include <plateau/geometry/mesh_extractor.h>

using namespace citygml;
using namespace plateau::geometry;

class MeshExtractorTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";

        ParserParams params;
        params.tesselate = true;

        city_model_ = load(gml_path_, params);
        logger_ = std::make_shared<PlateauDllLogger>();
    }

    std::string gml_path_;
    std::shared_ptr<const CityModel> city_model_;
    std::shared_ptr<PlateauDllLogger> logger_;
};

TEST_F(MeshExtractorTest, gridMerge_returns_polygons_with_vertices){
    auto meshExtractor = MeshExtractor();
    auto options = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5);
    auto result = meshExtractor.gridMerge(*city_model_, options, logger_);
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
    auto result = meshExtractor.gridMerge(*city_model_.get(), options, logger_);
    for(auto& poly : result){
        auto sizeOfUV1 = poly.getUV1().size();
        auto numOfVertices = poly.getVertices().size();
        ASSERT_EQ(sizeOfUV1, numOfVertices);
    }
}

TEST_F(MeshExtractorTest, extract_returns_model_with_child_and_name){
    auto meshExtractor = MeshExtractor();
    auto options = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5);
    auto model = meshExtractor.extract(*city_model_, options, logger_);
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
    std::string firstChildName = model->getRootNodeAt(0).getChildAt(0).getName();
    ASSERT_EQ(firstChildName, "grid0");
}

void test_extract_from_c_wrapper(){
    auto gml_path = "../data/udx/bldg/53392642_bldg_6697_op2.gml";

    ParserParams params;
    params.tesselate = true;

    auto city_model = load(gml_path, params);
    auto logger = std::make_shared<PlateauDllLogger>();
    MeshExtractor* meshExtractor;
    plateau_mesh_extractor_new(&meshExtractor);
    auto options = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5);
    auto model = meshExtractor->extract(*city_model, options, logger);
    auto nodes = model->getNodesRecursive();
    ASSERT_EQ(model->getRootNodeAt(0).getChildAt(0).getName(), "grid0");
    plateau_mesh_extractor_delete(meshExtractor);
}

TEST_F(MeshExtractorTest, extract_can_exec_twice){

    test_extract_from_c_wrapper();
    test_extract_from_c_wrapper();
}