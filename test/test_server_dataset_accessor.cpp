#include <gtest/gtest.h>
#include <plateau/dataset/i_dataset_accessor.h>
#include <plateau/dataset/dataset_source.h>

#include "plateau/network/client.h"

namespace plateau::dataset {
    using namespace plateau::geometry;

    class ServerDatasetAccessorTest : public ::testing::Test {
    };

    TEST_F(ServerDatasetAccessorTest, getGmlFiles_returns_value) { // NOLINT
        const auto dataset_source = DatasetSource::createServer("23ku", network::Client());
        const auto accessor = dataset_source.getAccessor();
        const auto gmls = accessor->getGmlFiles(PredefinedCityModelPackage::Building);
        ASSERT_EQ(gmls->size(), 2);
    }

    TEST_F(ServerDatasetAccessorTest, getPackages) { // NOLINT
        const auto dataset_source = DatasetSource::createServer("23ku", network::Client());
        const auto accessor = dataset_source.getAccessor();
        auto packages = accessor->getPackages();
        ASSERT_EQ((unsigned  long)0b1111, (unsigned long)packages);
    }

    TEST_F(ServerDatasetAccessorTest, getmaxLod) { // NOLINT
        const auto dataset_source = DatasetSource::createServer("23ku", network::Client());
        const auto accessor = dataset_source.getAccessor();
        int max_lod =
            accessor->filterByMeshCodes({ MeshCode("53392670") })
            ->getGmlFiles(PredefinedCityModelPackage::Building)
            ->at(0).getMaxLod();
        ASSERT_EQ(max_lod, 2);
    }
}
