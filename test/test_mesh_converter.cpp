#include <filesystem>
#include <gtest/gtest.h>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>

#include "plateau/io/mesh_converter.h"

namespace {
    void assertFileExists(const std::string& file_path)
    {
        std::ifstream ifs(file_path);
        ASSERT_TRUE(ifs.is_open());
        ifs.close();
    }
}

TEST(ObjWriter, OutputsObjAndMtl) {
    const auto path = "../data/udx/bldg/53392642_bldg_6697_op2.gml";
    const auto output_path = ".";


    const citygml::ParserParams parser_params;
    const auto city_model = citygml::load(path, parser_params, nullptr);

    MeshConverter().convert(output_path, path, city_model);

    const auto basename = std::filesystem::path(path).filename().replace_extension().string();

    std::vector expected_outputs = {
    std::filesystem::path(output_path).append("LOD0_" + basename + ".obj").string(),
    std::filesystem::path(output_path).append("LOD1_" + basename + ".obj").string(),
    std::filesystem::path(output_path).append("LOD2_" + basename + ".obj").string(),
    std::filesystem::path(output_path).append("LOD0_" + basename + ".mtl").string(),
    std::filesystem::path(output_path).append("LOD1_" + basename + ".mtl").string(),
    std::filesystem::path(output_path).append("LOD2_" + basename + ".mtl").string(),
    };

    for (const auto& output : expected_outputs)
    {
        assertFileExists(output);
        std::filesystem::remove(output);
    }
}
