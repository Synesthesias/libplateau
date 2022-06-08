#include <filesystem>
#include <gtest/gtest.h>

#include <obj_writer.h>
#include <citygml/citygml.h>
#include <citygml/citymodel.h>

TEST(ObjWriter, OutputsObj) {
    const auto path = "../data/udx/bldg/53392642_bldg_6697_op2.gml";
    const auto output_path = "output.obj";
    ObjWriter obj_writer;
    const citygml::ParserParams parser_params;
    const auto city_model = citygml::load(path, parser_params, nullptr);
    obj_writer.write(output_path, *city_model, path);

    std::ifstream ifs(output_path);
    EXPECT_TRUE(ifs.is_open());
    ifs.close();

    std::filesystem::remove(output_path);
}
