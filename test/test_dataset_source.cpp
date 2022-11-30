#include <gtest/gtest.h>
#include <plateau/dataset/dataset_source.h>

#include <memory>

namespace plateau::dataset {
    using namespace plateau::geometry;
    namespace fs = std::filesystem;

    class DatasetSourceTest : public ::testing::Test {

    };

    TEST_F(DatasetSourceTest, get_accessor_of_local_source_returns_local_accessor) { // NOLINT
        auto source =
                std::make_shared<DatasetSource>(
                        std::move(DatasetSource::createLocal(fs::path("../data"))));
        auto accessor = source->getAccessor();
        auto mesh_code_str = accessor->getMeshCodes().at(0).get();
        ASSERT_EQ(mesh_code_str, "53392642");
    }

    TEST_F(DatasetSourceTest, get_accessor_of_server_source_returns_server_accessor) { // NOLINT
        auto source = DatasetSource(DatasetSource::createServer(std::string("23ku")));
        auto accessor = source.getAccessor();
        auto mesh_code_str = accessor->getMeshCodes().at(0).get();
        ASSERT_EQ(mesh_code_str, "53392642");
        auto gml_files = accessor->getGmlFiles(Extent::all(), PredefinedCityModelPackage::Building);
        ASSERT_EQ(gml_files.at(0).getMeshCode().get(), "53392670");
    }
}
