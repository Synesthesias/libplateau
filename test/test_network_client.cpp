#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>
#include <plateau/network/client.h>

namespace fs = std::filesystem;
namespace plateau::network {

    class ClientTest : public ::testing::Test {
    protected:
        void SetUp() override {
            client.setApiServerUrl("https://9tkm2n.deta.dev");
        }

        Client client;
    };

    TEST_F(ClientTest, GetMetadata) {
        auto meta_data = client.getMetadata();

        ASSERT_EQ(meta_data[0].id, "tokyo");
        ASSERT_EQ(meta_data[0].datasets[0].max_lod, 3);
    }

    TEST_F(ClientTest, GetMeshCodes) {
        auto get_mesh_codes = client.getMeshCodes("23ku");

        ASSERT_EQ(get_mesh_codes->at(0).get(), "53392642");
    }

    TEST_F(ClientTest, GetFiles) {
        std::vector<plateau::udx::MeshCode> set_mesh_codes;
        set_mesh_codes.push_back(plateau::udx::MeshCode("53392642"));
        set_mesh_codes.push_back(plateau::udx::MeshCode("53392670"));
        auto file_urls = client.getFiles(set_mesh_codes);
        
        ASSERT_EQ(file_urls->begin()->second.at(0), "https://9tkm2n.deta.dev/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml");
    }

    TEST_F(ClientTest, Download) {
        std::string gml_file_name = "53392642_bldg_6697_2_op.gml";
        fs::remove(gml_file_name);
        auto fpath = client.download(".", "https://9tkm2n.deta.dev/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml");

        std::ifstream ifs(gml_file_name);
        ASSERT_TRUE(ifs.is_open());
        ifs.close();
    }
}