#include "gtest/gtest.h"
#include "citygml/citymodel.h"
#include "citygml/citygml.h"
#include "citygml/polygon.h"
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

TEST_F(MeshExtractorTest, extract){
    auto meshExtractor = MeshExtractor();
    auto options = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5);
    auto model = meshExtractor.extract(*city_model_, options, logger_);
    auto nodes = model->getNodesRecursive();
    for(auto node : nodes){
        auto& meshOpt = node->getMesh();
        std::string meshName = meshOpt ? meshOpt->getId() : "noneMesh";
        auto verticesCount = meshOpt ? meshOpt->getVertices().size() : 0;
        std::cout << node->getName() << " : meshId=" << meshName << ", verticesCount=" << verticesCount <<  std::endl;
    }
    // TODO
    ASSERT_TRUE(true);
}