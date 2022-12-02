#include <filesystem>
#include <gtest/gtest.h>
#include <plateau/network/client.h>
#include "plateau/network/network_config.h"

namespace fs = std::filesystem;
namespace plateau::network {

    class ClientTest : public ::testing::Test {
    protected:
        void SetUp() override {
            client.setApiServerUrl("https://9tkm2n.deta.dev");
        }

        Client client = Client();
    };

    TEST_F(ClientTest, GetMetadata) { // NOLINT
        auto meta_data = client.getMetadata();
        ASSERT_NE(meta_data.size(), 0);
        if (!meta_data.empty()) {
            ASSERT_EQ(meta_data[0].id, "tokyo");
            ASSERT_EQ(meta_data[0].datasets[0].max_lod, 3);
        }
    }

    TEST_F(ClientTest, GetMeshCodes) { // NOLINT
        auto get_mesh_codes = client.getMeshCodes("23ku");
        ASSERT_NE(get_mesh_codes.size(), 0);
        if (!get_mesh_codes.empty()) {
            ASSERT_EQ(get_mesh_codes.at(0).get(), "53392642");
        }
    }
    
    TEST_F(ClientTest, GetFiles) { // NOLINT
        std::vector<plateau::dataset::MeshCode> set_mesh_codes;
        set_mesh_codes.emplace_back("53392642");
        set_mesh_codes.emplace_back("53392670");
        auto file_url_lod = client.getFiles(set_mesh_codes);
        ASSERT_NE(file_url_lod->size(), 0);
        if (!file_url_lod->empty()) {
            ASSERT_EQ(file_url_lod->begin()->second[0].first, 1);
            ASSERT_EQ(file_url_lod->begin()->second[0].second, NetworkConfig::mockServerUrl() + "/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml");
        }
    }
    
    TEST_F(ClientTest, Download) { // NOLINT
        std::string gml_file_name = "53392642_bldg_6697_2_op.gml";
        fs::remove(gml_file_name);
        auto fpath = client.download(u8".", u8"https://9tkm2n.deta.dev/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml");

        ASSERT_TRUE(fs::file_size(fpath));
    }
}
