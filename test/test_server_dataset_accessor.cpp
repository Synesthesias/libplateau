#include <gtest/gtest.h>
#include <plateau/dataset/server_dataset_accessor.h>

namespace plateau::dataset{
    using namespace plateau::geometry;
    class ServerDatasetAccessorTest : public ::testing::Test{
    };

    TEST_F(ServerDatasetAccessorTest, getGmlFiles_returns_value){ // NOLINT
        auto accessor = ServerDatasetAccessor("23ku");
        auto gmls = accessor.getGmlFiles(Extent::all(), PredefinedCityModelPackage::Building);
        ASSERT_EQ(gmls.size(), 1);
    }

    TEST_F(ServerDatasetAccessorTest, getPackages){
        auto accessor = ServerDatasetAccessor("23ku");
        accessor.getGmlFiles(Extent::all(), PredefinedCityModelPackage::Building);
    }
}
