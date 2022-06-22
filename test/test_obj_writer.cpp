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
    const auto output_path = "output.obj";
    const auto mtl_path = std::filesystem::path(output_path).replace_extension("mtl").string();

    std::filesystem::remove(output_path);
    std::filesystem::remove(mtl_path);

    const citygml::ParserParams parser_params;
    const auto city_model = citygml::load(path, parser_params, nullptr);

    MeshConverter().convert(output_path, path, city_model);

    assertFileExists(output_path);
    assertFileExists(mtl_path);
}
