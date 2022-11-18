#include <gtest/gtest.h>
#include <plateau/udx/gml_file.h>
#include "plateau/udx/local_dataset_accessor.h"

namespace plateau::udx {
    class GmlFileTest : public ::testing::Test {

    };

    TEST_F(GmlFileTest, get_feature_type) { // NOLINT
        auto info = GmlFile("foobar/udx/bldg/53392546_bldg_6697_2_op.gml");
        ASSERT_EQ("bldg", info.getFeatureType());
        ASSERT_EQ(PredefinedCityModelPackage::Building, UdxSubFolder::getPackage("bldg"));
    }
}
