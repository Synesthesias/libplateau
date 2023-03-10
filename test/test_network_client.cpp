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

    TEST_F(ClientTest, GetMetadataOfDefaultServer) { // NOLINT
        auto client = Client("", "");
        auto metadata_groups = client.getMetadata();
        ASSERT_TRUE(!metadata_groups->empty()) << "Metadata groups is not empty.";
        auto metadata_group = metadata_groups->at(0);
        ASSERT_TRUE(!metadata_group.datasets.empty()) << "First metadata group has metadata.";
        ASSERT_TRUE(!metadata_group.title.empty()) << "First metadata group has title.";
        ASSERT_TRUE(!metadata_group.id.empty()) << "First metadata group has id.";
        auto metadata = metadata_group.datasets.at(0);
        ASSERT_TRUE(!metadata.title.empty()) << "Metadata has title.";
        ASSERT_TRUE(!metadata.id.empty()) << "Metadata has id.";
    }

    TEST_F(ClientTest, DownloadFilesOfDefaultServer) { // NOLINT
        auto client = Client("", "");
        auto metadata = client.getMetadata();
        ASSERT_TRUE(!metadata->empty());
        auto first_id = metadata->at(0).datasets.at(0).id;
        auto files = client.getFiles(first_id);
        ASSERT_TRUE(!files.empty()) << "Dataset files is not empty.";
        auto files_of_first_package = files.begin()->second;
        ASSERT_TRUE(!files_of_first_package.empty());
        auto file = files_of_first_package.at(0);
        ASSERT_TRUE(0 <= file.max_lod && file.max_lod <= 4 );
        ASSERT_TRUE(!file.mesh_code.empty());
        ASSERT_TRUE(!file.url.empty());
        auto url = file.url;
        fs::remove("./" + fs::path(url).filename().u8string());
        auto downloaded_path = fs::u8path(client.download("./", url));
        ASSERT_TRUE(fs::exists(downloaded_path));
        ASSERT_TRUE(fs::file_size(downloaded_path) > 0);
        fs::remove(downloaded_path);
    }
}
