#include <gtest/gtest.h>
#include <plateau/dataset/dataset_source.h>
#include <filesystem>
#include <memory>

#include "plateau/network/client.h"

namespace plateau::dataset {
    using namespace plateau::geometry;
    namespace fs = std::filesystem;

    class DatasetSourceTest : public ::testing::Test {

    };

    TEST_F(DatasetSourceTest, get_accessor_of_local_source_returns_local_accessor) { // NOLINT
        auto source =
                std::make_shared<DatasetSource>(
                        std::move(DatasetSource::createLocal(u8"../data/日本語パステスト")));
        auto accessor = source->getAccessor();
        auto mesh_code_str = accessor->getMeshCodes().begin()->get();
        ASSERT_EQ(mesh_code_str, "53392642");
    }

    TEST_F(DatasetSourceTest, get_accessor_of_server_source_returns_server_accessor) { // NOLINT
        auto source = DatasetSource::createServer(std::string("23ku"), network::Client());
        auto accessor = source.getAccessor();
        auto mesh_code_str = accessor->getMeshCodes().begin()->get();
        ASSERT_EQ(mesh_code_str, "533926");
        auto gml_files = accessor->getGmlFiles(PredefinedCityModelPackage::Building);
        ASSERT_EQ(gml_files->at(0).getMeshCode().get(), "53392642");
    }
}
