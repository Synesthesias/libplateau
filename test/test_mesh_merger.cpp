#include "gtest/gtest.h"
#include "citygml/citymodel.h"
#include "citygml/citygml.h"
#include "plateau/io/mesh_merger.h"

using namespace citygml;
class MeshMergerTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";

        ParserParams params;
        params.tesselate = true;

        city_model_ = load(gml_path_, params);
    }

    std::string gml_path_;
    std::shared_ptr<const CityModel> city_model_;
};

TEST_F(MeshMergerTest, meshMergerTest){
    // TODO
    MeshMerger::GridMerge(*city_model_, CityObject::CityObjectsType::COT_All, 5, 5);
}