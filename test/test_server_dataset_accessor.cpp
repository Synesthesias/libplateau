#include <gtest/gtest.h>
#include <plateau/dataset/server_dataset_accessor.h>
#include <plateau/dataset/dataset_source.h>

namespace plateau::dataset {
    using namespace plateau::geometry;

    class ServerDatasetAccessorTest : public ::testing::Test {
    };

    TEST_F(ServerDatasetAccessorTest, getGmlFiles_returns_value) { // NOLINT
        auto accessor = ServerDatasetAccessor("23ku");
        auto gmls = accessor.getGmlFiles(Extent::all(), PredefinedCityModelPackage::Building);
        ASSERT_EQ(gmls.size(), 2);
    }

    TEST_F(ServerDatasetAccessorTest, getPackages) { // NOLINT
        auto accessor = ServerDatasetAccessor("23ku");
        accessor.getGmlFiles(Extent::all(), PredefinedCityModelPackage::Building);
        auto packages = accessor.getPackages();
        ASSERT_EQ((unsigned  long)0b1111, (unsigned long)packages);
    }

    TEST_F(ServerDatasetAccessorTest, getFeatureTypes) { // NOLINT
        auto accessor = ServerDatasetAccessor("");
        auto metadata_groups = accessor.getDatasetMetadataGroup();
        auto dataset = metadata_groups.at(0).datasets.at(1);
        ASSERT_EQ(dataset.title, u8"八王子市");
        ASSERT_EQ(dataset.feature_types.at(0), u8"bldg");
        ASSERT_EQ(dataset.feature_types.at(1), u8"dem");
    }

    TEST_F(ServerDatasetAccessorTest, getmaxLod) { // NOLINT
        auto source = DatasetSource::createServer("23ku");
        auto accessor = source.getAccessor();
        int max_lod = accessor->getMaxLod(MeshCode("53392670"), PredefinedCityModelPackage::Building);
//        ASSERT_EQ(max_lod, 2);
//        int not_found_lod = accessor->getMaxLod(MeshCode("00000000"), PredefinedCityModelPackage::Unknown);
//        ASSERT_EQ(not_found_lod, -1);
    }
}
