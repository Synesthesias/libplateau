#include <filesystem>
#include <gtest/gtest.h>
#include <plateau/network/client.h>

namespace fs = std::filesystem;
namespace plateau::network {

    class ClientTest : public ::testing::Test {
    protected:
        void SetUp() override {
//            client.setApiServerUrl(Client::getDefaultServerUrl());
        }

//        Client client = Client();
    };

    TEST_F(ClientTest, GetMetadata) { // NOLINT
        auto meta_data = Client::createClientForMockServer().getMetadata();
        ASSERT_NE(meta_data->size(), 0) << "Metadata Exists.";
        if (!meta_data->empty()) {
            ASSERT_EQ(meta_data->at(0).id, "tokyo");
        }
    }

    TEST_F(ClientTest, GetFiles){ // NOLINT
        auto files = Client::createClientForMockServer().getFiles("23ku");
        ASSERT_TRUE(!files.empty());
    }
    
    TEST_F(ClientTest, Download) { // NOLINT
        std::string gml_file_name = "53392642_bldg_6697_2_op.gml";
        fs::remove(gml_file_name);
        auto fpath_str = Client::createClientForMockServer().download(u8".", Client::getMockServerUrl() + u8"/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml");
        auto fpath = fs::u8path(fpath_str);
        ASSERT_TRUE(fs::file_size(fpath));
    }
}
