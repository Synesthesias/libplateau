#include <filesystem>
#include <gtest/gtest.h>
#include <plateau/network/client.h>

namespace fs = std::filesystem;
namespace plateau::network {

    class ClientTest : public ::testing::Test {
    protected:
        void SetUp() override {
        }

    };

    TEST_F(ClientTest, GetMetadataOfMockServer) { // NOLINT
        auto metadata_groups = Client::createClientForMockServer().getMetadata();
        ASSERT_NE(metadata_groups->size(), 0) << "Metadata Exists.";
        if (!metadata_groups->empty()) {
            ASSERT_EQ(metadata_groups->at(0).id, "tokyo");
        }
    }

    TEST_F(ClientTest, GetFilesOfMockServer){ // NOLINT
        auto files = Client::createClientForMockServer().getFiles("23ku");
        ASSERT_TRUE(!files.empty());
    }
    
    TEST_F(ClientTest, DownloadFromMockServer) { // NOLINT
        std::string gml_file_name = "53392642_bldg_6697_2_op.gml";
        fs::remove(gml_file_name);
        auto fpath_str = Client::createClientForMockServer().download(u8".", Client::getMockServerUrl() + u8"/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml");
        auto fpath = fs::u8path(fpath_str);
        ASSERT_TRUE(fs::file_size(fpath));
    }
}
