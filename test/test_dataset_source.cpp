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
        auto grid_codes = accessor->getGridCodes();
        bool found1 = false;
        bool found2 = false;
        for (const auto& grid_code : grid_codes) {
            if (grid_code->get() == "08EE763") { // 国土基本図の図郭
                found1 = true;
            }
            if(grid_code->get() == "533925") { // メッシュコード
                found2 = true;
            }
        }
        ASSERT_TRUE(found1);
        ASSERT_TRUE(found2);
    }

    TEST_F(DatasetSourceTest, DISABLED_get_accessor_of_server_source_returns_server_accessor) { // NOLINT
        auto source = DatasetSource::createServer(std::string("23ku"), network::Client::createClientForMockServer());
        auto accessor = source.getAccessor();
        auto grid_code_str = accessor->getGridCodes().begin()->get()->get();
        ASSERT_EQ(grid_code_str, "533926");
        auto gml_files = accessor->getGmlFiles(PredefinedCityModelPackage::Building);
        ASSERT_EQ(gml_files->at(0).getGridCode()->get(), "53392642");
    }
}
