
#include "test_granularity_converter.h"
#include "citygml/citygml.h"
#include <queue>
#include "../src/c_wrapper/granularity_converter_c.cpp"

using namespace plateau::granularityConvert;
using namespace plateau::polygonMesh;


TEST_F(GranularityConverterTest, convertAreaToAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfArea().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToPrimary) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfArea().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToArea) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfArea().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAreaRootToAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfArea_OnlyRoot().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaRootToPrimaruy) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfArea_OnlyRoot().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaRootToArea) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfArea_OnlyRoot().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToArea) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfAtomic().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToPrimary) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfAtomic().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAtomicToAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfAtomic().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToArea) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfPrimary().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertPrimaryToPrimary) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfPrimary().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfPrimary().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToAtomic_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfArea_OnlyAtomicMesh().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToPrimary_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfArea_OnlyAtomicMesh().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToArea_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfArea_OnlyAtomicMesh().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToArea_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfAtomic_OnlyAtomicMesh().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToPrimary_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfAtomic_OnlyAtomicMesh().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAtomicToAtomic_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfAtomic_OnlyAtomicMesh().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToArea_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfPrimary_OnlyAtomicMesh().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertPrimaryToPrimary_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfPrimary_OnlyAtomicMesh().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToAtomic_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestModelOfPrimary_OnlyAtomicMesh().test(MeshGranularity::PerAtomicFeatureObject);
}

namespace {
    std::shared_ptr<Model> loadTestGML() {
        auto parser_params = citygml::ParserParams();
        auto city_model = citygml::load(u8"../data/日本語パステスト/udx/bldg/53392642_bldg_6697_op2.gml", parser_params);
        auto extract_option = MeshExtractOptions();
        auto src_model = MeshExtractor::extract(*city_model, extract_option);
        return src_model;
    }
}

TEST_F(GranularityConverterTest, ConvertFromTestGmlFile) { // NOLINT
    auto src_model = loadTestGML();
    auto convert_option_area = GranularityConvertOption(MeshGranularity::PerCityModelArea, 10);
    auto convert_option_feature = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
    auto dst_model_area = GranularityConverter().convert(*src_model, convert_option_area);
    auto dst_model_feature = GranularityConverter().convert(*src_model, convert_option_feature);
}

TEST_F(GranularityConverterTest, CWrapperWorks) { // NOLINT
    auto src_model = loadTestGML();
    auto convert_option = GranularityConvertOption(MeshGranularity::PerCityModelArea, 10);
    Model* out_model_ptr;
    plateau_granularity_converter_convert(
            src_model.get(), &out_model_ptr, convert_option);
}
