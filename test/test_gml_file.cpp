#include <gtest/gtest.h>
#include <plateau/dataset/gml_file.h>
#include "plateau/dataset/i_dataset_accessor.h"

namespace plateau::dataset {
    class GmlFileTest : public ::testing::Test {

    };

    TEST_F(GmlFileTest, get_feature_type) { // NOLINT
        auto info = GmlFile(std::string("foobar/udx/bldg/53392546_bldg_6697_2_op.gml"));
        ASSERT_EQ("bldg", info.getFeatureType());
        ASSERT_EQ(PredefinedCityModelPackage::Building, UdxSubFolder::getPackage("bldg"));
    }

    TEST_F(GmlFileTest, get_epsg) { // NOLINT
        auto info1 = GmlFile(std::string("foobar/udx/unf/08EE751_unf_10169_water_op.gml"));
        ASSERT_EQ(10169, info1.getEpsg());
        ASSERT_FALSE(info1.isPolarCoordinateSystem());

        auto info2 = GmlFile(std::string("foobar/udx/bldg/53392546_bldg_6697_2_op.gml"));
        ASSERT_EQ(6697, info2.getEpsg());
        ASSERT_TRUE(info2.isPolarCoordinateSystem());     
    }

    // fetch のテストは test_dataset.cpp にあります。

}
