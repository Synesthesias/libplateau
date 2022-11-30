#include <gtest/gtest.h>
#include <plateau/dataset/server_dataset_accessor.h>

namespace plateau::dataset {
    using namespace plateau::geometry;

    class ServerDatasetAccessorTest : public ::testing::Test {
    };

    TEST_F(ServerDatasetAccessorTest, getGmlFiles_returns_value) { // NOLINT
        auto accessor = ServerDatasetAccessor("23ku");
        auto gmls = accessor.getGmlFiles(Extent::all(), PredefinedCityModelPackage::Building);
        ASSERT_EQ(gmls.size(), 1);
    }

    TEST_F(ServerDatasetAccessorTest, getPackages) { // NOLINT
        auto accessor = ServerDatasetAccessor("23ku");
        accessor.getGmlFiles(Extent::all(), PredefinedCityModelPackage::Building);
    }

    TEST_F(ServerDatasetAccessorTest, getFeatureTypes) { // NOLINT
        auto accessor = ServerDatasetAccessor("");
        auto metadata_groups = accessor.getDatasetMetadataGroup();
        auto dataset = metadata_groups.at(0).datasets.at(1);
        ASSERT_EQ(dataset.title, u8"八王子市");
        ASSERT_EQ(dataset.feature_types.at(0), u8"bldg");
        ASSERT_EQ(dataset.feature_types.at(1), u8"dem");
    }
}
