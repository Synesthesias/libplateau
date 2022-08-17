#include "gtest/gtest.h"
#include "citygml/citymodel.h"
#include "citygml/citygml.h"
#include "citygml/polygon.h"
#include "plateau/io/mesh_merger.h"

using namespace citygml;
class MeshMergerTest : public ::testing::Test {
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

TEST_F(MeshMergerTest, gridMerge_returns_polygons_with_vertices){
    auto meshMerger = MeshMerger();
    meshMerger.gridMerge(city_model_.get(), CityObject::CityObjectsType::COT_All, 5, 5, logger_);
    auto result = meshMerger.getLastGridMergeResult();
    int numPolyWithVert = 0;
    for(auto poly : *result){
        if(!poly->getVertices().empty()){
            numPolyWithVert++;
        }
    }
    ASSERT_TRUE(numPolyWithVert >= 5);
}

TEST_F(MeshMergerTest, gridMerge_uv1_size_matches_num_of_vertices){
    auto meshMerger = MeshMerger();
    meshMerger.gridMerge(city_model_.get(), CityObject::CityObjectsType::COT_All, 5, 5, logger_);
    auto result = meshMerger.getLastGridMergeResult();
    for(auto poly : *result){
        auto sizeOfUV1 = poly->getUV1().size();
        auto numOfVertices = poly->getVertices().size();
        ASSERT_EQ(sizeOfUV1, numOfVertices);
    }
}