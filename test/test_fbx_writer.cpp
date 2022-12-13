#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>

#include <plateau/polygon_mesh/mesh_extractor.h>
#include <plateau/mesh_writer/fbx_writer.h>

using namespace citygml;
namespace fs = std::filesystem;
namespace plateau::meshWriter {

    class FbxWriterTest : public ::testing::Test {
    protected:
        void SetUp() override {
            params_.tesselate = true;
            fs::remove_all(u8"./tempTestDestDir");
        }

        void TearDown() override {
            fs::remove_all(u8"./tempTestDestDir");
        }

        ParserParams params_;


        const std::string gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";
        std::string output_directory_ = ".";
        std::string basename_ = fs::path(gml_path_).filename().replace_extension().string();
        fs::path expected_output_gltf_ = fs::path(output_directory_).append(basename_ + ".fbx");

        plateau::polygonMesh::MeshExtractOptions mesh_extract_options_;
        const std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
        const std::shared_ptr<plateau::polygonMesh::Model> model_ = plateau::polygonMesh::MeshExtractor::extract(
                *city_model_, mesh_extract_options_);

        static void assertFileExists(const fs::path& file_path);
        bool exportFbx(FbxFileFormat file_format, const fs::path& output_dir, const fs::path& output_gltf_path,
                        const fs::path& output_texture_dir);
    };

    void FbxWriterTest::assertFileExists(const fs::path& file_path) {
        std::ifstream ifs(file_path);
        ASSERT_TRUE(ifs.is_open());
        ifs.close();
    }

    TEST_F(FbxWriterTest, OutputsFile) { // NOLINT
        FbxWriteOptions gltf_options;
        gltf_options.file_format = FbxFileFormat::Binary;

        fs::remove(expected_output_gltf_);

        auto result = FbxWriter().write(expected_output_gltf_.string(), *model_, gltf_options);

        ASSERT_TRUE(result);
        assertFileExists(expected_output_gltf_);
    }
}
