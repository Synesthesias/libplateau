#include <filesystem>
#include <gtest/gtest.h>

#include <citygml/citygml.h>

#include <plateau/polygon_mesh/primary_city_object_types.h>

#include "citygml/citymodel.h"

using namespace citygml;

class PrimaryFeaturesTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";

        ParserParams params;
        params.tesselate = false;

        city_model_ = load(gml_path_, params);
    }

    std::string gml_path_;
    std::shared_ptr<const CityModel> city_model_;
};

TEST_F(PrimaryFeaturesTest, checkIfCityObjectIsPrimary) {
    ASSERT_TRUE(PrimaryCityObjectTypes::isPrimary(CityObject::CityObjectsType::COT_Building));
    ASSERT_TRUE(PrimaryCityObjectTypes::isPrimary(CityObject::CityObjectsType::COT_BuildingPart));
    ASSERT_TRUE(PrimaryCityObjectTypes::isPrimary(CityObject::CityObjectsType::COT_Road));
    ASSERT_TRUE(PrimaryCityObjectTypes::isPrimary(
            CityObject::CityObjectsType::COT_Bridge | CityObject::CityObjectsType::COT_LandUse));

    ASSERT_FALSE(PrimaryCityObjectTypes::isPrimary(CityObject::CityObjectsType::COT_RoofSurface));
    ASSERT_FALSE(PrimaryCityObjectTypes::isPrimary(
            CityObject::CityObjectsType::COT_Door | CityObject::CityObjectsType::COT_Window));
}
