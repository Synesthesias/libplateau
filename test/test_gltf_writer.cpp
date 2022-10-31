#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>

#include <plateau/polygon_mesh/mesh_extractor.h>
#include <plateau/mesh_writer/gltf_writer.h>

using namespace citygml;
namespace fs = std::filesystem;
namespace plateau::meshWriter {

    class GltfWriterTest : public ::testing::Test {
    protected:
        void SetUp() override {
            params_.tesselate = true;
        }
        ParserParams params_;
        

        const std::string gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";
        std::string output_directory_ = ".";
        std::string basename_ = fs::path(gml_path_).filename().replace_extension().string();
        std::string expected_output_gltf_ = fs::path(output_directory_).append(basename_).string() + ".gltf";
        std::string expected_output_bin_ = fs::path(output_directory_).append(basename_).string() + ".bin";

        plateau::polygonMesh::MeshExtractOptions mesh_extract_options_;
        const std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
        const std::shared_ptr<plateau::polygonMesh::Model> model_ = plateau::polygonMesh::MeshExtractor::extract(*city_model_, mesh_extract_options_);
        
        void assertFileExists(const std::string& file_path);
    };

    TEST_F(GltfWriterTest, OutputsGltfAndBin) {
        GltfWriteOptions gltf_options;
        gltf_options.mesh_file_format = GltfFileFormat::GLTF;
        gltf_options.texture_directory_path = "./texture";

        fs::remove(expected_output_gltf_);
        fs::remove(expected_output_bin_);

        auto result = GltfWriter().write(expected_output_gltf_, *model_, gltf_options);

        assertFileExists(expected_output_gltf_);
        assertFileExists(expected_output_bin_);
    }


    TEST_F(GltfWriterTest, export_path_can_contain_multibyte_chars){ // NOLINT
        fs::remove_all(u8"./tempTestDestDir");
        auto output_dir = fs::u8path(u8"./tempTestDestDir/日本語対応テスト");
        fs::create_directories(output_dir);
        auto texture_dir = fs::u8path(u8"./tempTestDtDir/日本語対応テスト/tex");
        auto expected_output_gltf = fs::path(output_dir).append(basename_ + ".gltf");
        auto expected_output_bin = fs::path(output_dir).append(basename_ + ".bin");
        GltfWriteOptions gltf_options;
        gltf_options.mesh_file_format = GltfFileFormat::GLTF;
        gltf_options.texture_directory_path = texture_dir.u8string();

        auto result = GltfWriter().write(expected_output_gltf.u8string(), *model_, gltf_options);

        ASSERT_TRUE(result);
        assertFileExists(expected_output_gltf.string());
        assertFileExists(expected_output_bin.string());
        fs::remove_all(u8"./tempTestDestDir");
    }

    void GltfWriterTest::assertFileExists(const std::string& file_path) {
        std::ifstream ifs(file_path);
        ASSERT_TRUE(ifs.is_open());
        ifs.close();
    }
}
