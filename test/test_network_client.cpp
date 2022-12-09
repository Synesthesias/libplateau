#include <filesystem>
#include <gtest/gtest.h>
#include <plateau/network/client.h>

namespace fs = std::filesystem;
namespace plateau::network {

    class ClientTest : public ::testing::Test {
    protected:
        void SetUp() override {
            client.setApiServerUrl(Client::getDefaultServerUrl());
        }

        Client client = Client();
    };

    TEST_F(ClientTest, GetMetadata) { // NOLINT
        auto meta_data = client.getMetadata();
        ASSERT_NE(meta_data->size(), 0);
        if (!meta_data->empty()) {
            ASSERT_EQ(meta_data->at(0).id, "tokyo");
            ASSERT_EQ(meta_data->at(0).datasets[0].max_lod, 3);
        }
    }
    
    TEST_F(ClientTest, Download) { // NOLINT
        std::string gml_file_name = "53392642_bldg_6697_2_op.gml";
        fs::remove(gml_file_name);
        auto fpath = client.download(u8".", Client::getDefaultServerUrl() + u8"/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml");

        ASSERT_TRUE(fs::file_size(fpath));
    }
}
