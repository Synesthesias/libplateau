
#include "test_granularity_converter.h"
#include "citygml/citygml.h"
#include <queue>
#include "../src/c_wrapper/granularity_converter_c.cpp"

using namespace plateau::granularityConvert;
using namespace plateau::polygonMesh;


TEST_F(GranularityConverterTest, convertAreaToAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToPrimary) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToArea) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAreaToAtomic_OnlyRoot) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea_OnlyRoot().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToPrimary_OnlyRoot) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea_OnlyRoot().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToArea_OnlyRoot) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea_OnlyRoot().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertPrimaryToAtomic_OnlyRoot) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_OnlyRoot().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToPrimaruy_OnlyRoot) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_OnlyRoot().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToArea_OnlyRoot) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_OnlyRoot().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToAtomic_OnlyRoot) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_OnlyRoot().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAtomicToPrimaruy_OnlyRoot) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_OnlyRoot().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAtomicToArea_OnlyRoot) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_OnlyRoot().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAreaToAtomic_MultipleGMLs) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea_MultipleGMLs().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToPrimary_MultipleGMLs) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea_MultipleGMLs().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToArea_MultipleGMLs) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea_MultipleGMLs().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToArea) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToPrimary) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAtomicToAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToArea) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertPrimaryToPrimary) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToAtomic_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsFromArea_OnlyAtomicMesh().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToPrimary_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsFromArea_OnlyAtomicMesh().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToArea_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsFromArea_OnlyAtomicMesh().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToArea_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_OnlyAtomicMesh().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToPrimary_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_OnlyAtomicMesh().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAtomicToAtomic_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_OnlyAtomicMesh().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToArea_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_OnlyAtomicMesh().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertPrimaryToPrimary_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_OnlyAtomicMesh().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToAtomic_OnlyAtomic) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_OnlyAtomicMesh().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToArea_WithoutCityObjList) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea_WithoutCityObjList().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAreaToPrimary_WithoutCityObjList) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea_WithoutCityObjList().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToAtomic_WithoutcityObjList) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea_WithoutCityObjList().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToArea_WithoutCityObjList) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_WithoutCityObjList().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertPrimaryToPrimary_WithoutCityObjList) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_WithoutCityObjList().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToAtomic_WithoutcityObjList) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_WithoutCityObjList().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAtomicToArea_WithoutCityObjList) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_WithoutCityObjList().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToPrimary_WithoutCityObjList) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_WithoutCityObjList().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAtomicToAtomic_WithoutcityObjList) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_WithoutCityObjList().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToArea_OnlyAtomic_Root) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfArea_OnlyAtomicMesh_Root().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToArea_OnlyAtomic_Root) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_OnlyAtomicMesh_Root().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToPrimary_OnlyAtomic_Root) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_OnlyAtomicMesh_Root().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAtomicToAtomic_OnlyAtomic_Root) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfAtomic_OnlyAtomicMesh_Root().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToArea_OnlyAtomic_Root) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_OnlyAtomicMesh_Root().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertPrimaryToPrimary_OnlyAtomic_Root) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_OnlyAtomicMesh_Root().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToAtomic_OnlyAtomic_Root) { // NOLINT
    ModelConvertTestPatternsFactory().createTestPatternsOfPrimary_OnlyAtomicMesh_Root().test(MeshGranularity::PerAtomicFeatureObject);
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
