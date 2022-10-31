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
        fs::path expected_output_gltf_ = fs::path(output_directory_).append(basename_ + ".gltf");
        fs::path expected_output_bin_ = fs::path(output_directory_).append(basename_ + ".bin");

        plateau::polygonMesh::MeshExtractOptions mesh_extract_options_;
        const std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
        const std::shared_ptr<plateau::polygonMesh::Model> model_ = plateau::polygonMesh::MeshExtractor::extract(
                *city_model_, mesh_extract_options_);

        void assertFileExists(const fs::path& file_path);
        bool exportGltf(GltfFileFormat file_format, fs::path output_dir, fs::path output_gltf_path,
                        fs::path output_texture_dir);
    };

    TEST_F(GltfWriterTest, OutputsGltfAndBin) { // NOLINT
        GltfWriteOptions gltf_options;
        gltf_options.mesh_file_format = GltfFileFormat::GLTF;
        gltf_options.texture_directory_path = "./texture";

        fs::remove(expected_output_gltf_);
        fs::remove(expected_output_bin_);

        auto result = GltfWriter().write(expected_output_gltf_.string(), *model_, gltf_options);

        ASSERT_TRUE(result);
        assertFileExists(expected_output_gltf_);
        assertFileExists(expected_output_bin_);
    }


    TEST_F(GltfWriterTest, gltf_export_path_can_contain_multibyte_chars) { // NOLINT
        fs::remove_all(u8"./tempTestDestDir");

        auto output_dir = fs::u8path(u8"./tempTestDestDir/日本語対応テスト");
        auto expected_output_gltf = fs::path(output_dir).append(basename_ + ".gltf");
        auto expected_output_bin = fs::path(output_dir).append(basename_ + ".bin");

        auto result = exportGltf(
                GltfFileFormat::GLTF, output_dir, expected_output_gltf,
                fs::u8path(u8"./tempTestDtDir/日本語対応テスト/テクスチャ"));


        ASSERT_TRUE(result);
        assertFileExists(expected_output_gltf);
        assertFileExists(expected_output_bin);
        fs::remove_all(u8"./tempTestDestDir");
    }

    TEST_F(GltfWriterTest, glb_export_path_can_contain_multibyte_chars) { // NOLINT
        fs::remove_all(u8"./tempTestDestDir");
        auto output_dir = fs::u8path(u8"./tempTestDestDir/日本語対応テスト");
        fs::create_directories(output_dir);
        auto expected_output_glb = fs::path(output_dir).append(basename_ + ".glb");

        auto result = exportGltf(
                GltfFileFormat::GLB,
                output_dir, expected_output_glb,
                fs::u8path(u8"./tempTestDtDir/日本語対応テスト/テクスチャ"));

        ASSERT_TRUE(result);
        assertFileExists(expected_output_glb);
        fs::remove_all(u8"./tempTestDestDir");
    }

    void GltfWriterTest::assertFileExists(const fs::path& file_path) {
        std::ifstream ifs(file_path);
        ASSERT_TRUE(ifs.is_open());
        ifs.close();
    }

    bool GltfWriterTest::exportGltf(GltfFileFormat file_format, fs::path output_dir, fs::path output_gltf_path,
                                    fs::path texture_dir) {
        fs::create_directories(output_dir);

        GltfWriteOptions gltf_options;
        gltf_options.mesh_file_format = file_format;
        gltf_options.texture_directory_path = texture_dir.u8string();

        auto is_succeed = GltfWriter().write(output_gltf_path.u8string(), *model_, gltf_options);
        return is_succeed;
    }
}
