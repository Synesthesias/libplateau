#include <gtest/gtest.h>
#include <plateau/dataset/i_dataset_accessor.h>
#include <plateau/dataset/dataset_source.h>

#include "plateau/network/client.h"

namespace plateau::dataset {
    using namespace plateau::geometry;

    class ServerDatasetAccessorTest : public ::testing::Test {
    };

    TEST_F(ServerDatasetAccessorTest, getGmlFiles_returns_value) { // NOLINT
        const auto dataset_source = DatasetSource::createServer("23ku", network::Client::createClientForMockServer());
        const auto accessor = dataset_source.getAccessor();
        const auto gmls = accessor->getGmlFiles(PredefinedCityModelPackage::Building);
        ASSERT_EQ(gmls->size(), 2);
    }

    TEST_F(ServerDatasetAccessorTest, getPackages) { // NOLINT
        const auto dataset_source = DatasetSource::createServer("23ku", network::Client::createClientForMockServer());
        const auto accessor = dataset_source.getAccessor();
        auto packages = accessor->getPackages();
        ASSERT_EQ((unsigned  long)0b1001111, (unsigned long)packages);
    }

    TEST_F(ServerDatasetAccessorTest, getmaxLod) { // NOLINT
        const auto dataset_source = DatasetSource::createServer("23ku", network::Client::createClientForMockServer());
        const auto accessor = dataset_source.getAccessor();
        const auto filter_by_mesh_codes = accessor->filterByMeshCodes({MeshCode("53392670")});
        const auto gml_files = filter_by_mesh_codes->getGmlFiles(PredefinedCityModelPackage::Building);
        auto first_gml = gml_files->at(0);
        int max_lod = first_gml.getMaxLod();
        ASSERT_EQ(max_lod, 2);
    }
}
