#include <gtest/gtest.h>
#include <plateau/udx/gml_file_info.h>
#include "plateau/udx/udx_file_collection.h"

namespace plateau::udx{
    class GmlFileInfoTest : public ::testing::Test {

    };

    TEST_F(GmlFileInfoTest, get_feature_type) { // NOLINT
        auto info = GmlFileInfo("foobar/udx/bldg/53392546_bldg_6697_2_op.gml");
        ASSERT_EQ("bldg", info.getFeatureType());
        ASSERT_EQ(PredefinedCityModelPackage::Building, UdxSubFolder::getPackage("bldg"));
    }
}
