#include <filesystem>
#include <gtest/gtest.h>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>

#include <plateau/polygon_mesh/mesh_extractor.h>
#include <plateau/mesh_writer/obj_writer.h>

using namespace citygml;
namespace fs = std::filesystem;

namespace plateau::meshWriter {
    class ObjWriterTest : public ::testing::Test {
    protected:
        void SetUp() override {
            params_.tesselate = true;
        }
        ParserParams params_;

        const std::string gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";
        std::string output_directory_ = ".";
        std::string basename_ = fs::path(gml_path_).filename().replace_extension().string();
        std::string expected_output_obj_ = fs::path(output_directory_).append(basename_).string() + ".obj";
        std::string expected_output_mtl_ = fs::path(output_directory_).append(basename_).string() + ".mtl";

        plateau::polygonMesh::MeshExtractOptions options;

        //CoordinateSystem::WUN,
        //        plateau::polygonMesh::MeshGranularity::PerPrimaryFeatureObject, 2, 2, true, 5, 1.0,
        //        Extent(GeoCoordinate(-90, -180, -999), GeoCoordinate(90, 180, 999)));
        const std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
        const std::shared_ptr<plateau::polygonMesh::Model> model_ = plateau::polygonMesh::MeshExtractor::extract(*city_model_, options);

        void assertFileExists(const std::string& file_path);
    };

    TEST_F(ObjWriterTest, OutputsObjAndMtl) { // NOLINT
        fs::remove(expected_output_obj_);
        fs::remove(expected_output_mtl_);

        auto result = ObjWriter().write(expected_output_obj_, *model_);

        assertFileExists(expected_output_obj_);
        assertFileExists(expected_output_mtl_);
    }

    TEST_F(ObjWriterTest, when_export_path_contains_multibyte_chars){ // NOLINT
        auto output_dir = fs::u8path(u8"./tempTestDestDir_日本語パス");
        auto expected_output_obj = output_dir.append(basename_ + ".obj");
        auto result = ObjWriter().write(expected_output_obj.string(), *model_);
        ASSERT_TRUE(result);
        assertFileExists(expected_output_obj.string());
    }

    void ObjWriterTest::assertFileExists(const std::string& file_path) {
        std::ifstream ifs(file_path);
        ASSERT_TRUE(ifs.is_open());
        ifs.close();
    }
}
