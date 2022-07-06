#include <filesystem>
#include <gtest/gtest.h>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>

#include "plateau/io/mesh_converter.h"

namespace {
    void removeFiles(const std::vector<std::string>& files) {
        for (const auto& file : files) {
            std::filesystem::remove(file);
        }
    }

    void assertFileExists(const std::string& file_path) {
        std::ifstream ifs(file_path);
        ASSERT_TRUE(ifs.is_open());
        ifs.close();
    }

}

class MeshConverterTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        gml_path_ = "../data/udx/bldg/53392642_bldg_6697_op2.gml";
        output_directory_ = ".";

        basename_ = std::filesystem::path(gml_path_).filename().replace_extension().string();
        expected_outputs_ = {
        std::filesystem::path(output_directory_).append("LOD0_" + basename_).string(),
        std::filesystem::path(output_directory_).append("LOD1_" + basename_).string(),
        std::filesystem::path(output_directory_).append("LOD2_" + basename_).string(),
        };

        removeFiles(getExpectedObjFiles());
        removeFiles(getExpectedMtlFiles());
    }

    std::vector<std::string> getExpectedObjFiles() const {
        std::vector<std::string> files;
        for (const auto& output : expected_outputs_) {
            files.push_back(output + ".obj");
        }
        return files;
    }

    std::vector<std::string> getExpectedMtlFiles() const {
        std::vector<std::string> files;
        for (const auto& output : expected_outputs_) {
            files.push_back(output + ".mtl");
        }
        return files;
    }

    MeshConverter converter_;

    std::string gml_path_;
    std::string output_directory_;
    std::string basename_;
    std::vector<std::string> expected_outputs_;
};

TEST_F(MeshConverterTest, OutputsObjAndMtl) {
    converter_.convert(output_directory_, gml_path_);

    for (const auto& output : getExpectedObjFiles()) {
        assertFileExists(output);
    }
    for (const auto& output : getExpectedMtlFiles()) {
        assertFileExists(output);
    }
}

TEST_F(MeshConverterTest, CanGetConvertedFiles) {
    std::vector<std::string> converted_files;
    converter_.convert(output_directory_, gml_path_, converted_files);

    for (const auto& output : getExpectedObjFiles()) {
        const auto expected = std::filesystem::path(output).make_preferred().string();
        const auto it = std::find(converted_files.cbegin(), converted_files.cend(), expected);
        const auto found_string = it != converted_files.cend()
            ? *it : "";
        ASSERT_EQ(expected, found_string);
        if (it != converted_files.cend()) {
            converted_files.erase(it);
        }
    }
    ASSERT_TRUE(converted_files.empty());
}