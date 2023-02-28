#include <filesystem>
#include <gtest/gtest.h>

#include <citygml/citygml.h>

#include <plateau/dataset/dataset_source.h>
#include <plateau/dataset/i_dataset_accessor.h>

using namespace citygml;
using namespace plateau::dataset;
using namespace plateau::network;
namespace fs = std::filesystem;

class DatasetTest : public ::testing::Test {
protected:
    void SetUp() override {
        source_path_ = "../data/日本語パステスト";
        local_dataset_accessor = DatasetSource::createLocal(source_path_).getAccessor();
    }

    static void checkFiles(const std::vector<std::string>& expected, const std::vector<GmlFile>& actual) {
        std::vector<std::string> actual_file_names;
        std::transform(actual.begin(), actual.end(), std::back_inserter(actual_file_names),
            [](const GmlFile& file_info) { return file_info.getPath(); });
        checkVectors(expected, actual_file_names);
    }

    static void checkVectors(const std::vector<std::string>& expected, const std::vector<std::string>& actual) {
        auto remain = expected;
        for (const auto& element : actual) {
            const auto it = std::find(remain.cbegin(), remain.cend(), element);
            if (it == remain.cend()) {
                FAIL() << "Unexpected element:" << element << " exists.";
            }
            remain.erase(it);
        }
        ASSERT_TRUE(remain.empty()) << remain[0] << " not found.";
    }

    std::string source_path_;
    std::shared_ptr<IDatasetAccessor> local_dataset_accessor;
};

TEST_F(DatasetTest, getGmlsLocal) { // NOLINT
    const std::vector expected_files =
    { std::filesystem::u8path(u8"../data/日本語パステスト/udx/bldg/53392642_bldg_6697_op2.gml").make_preferred().u8string(),
      std::filesystem::u8path( u8"../data/日本語パステスト/udx/tran/533925_tran_6697_op.gml").make_preferred().u8string() };
    std::vector<std::string> actual_files;
    auto packages = PredefinedCityModelPackage::Building | PredefinedCityModelPackage::Road;
    const auto gml_files = local_dataset_accessor->getGmlFiles(packages);
    for (const auto& gml_file : *gml_files) {
        actual_files.push_back(gml_file.getPath());
    }
    checkVectors(expected_files, actual_files);
}

TEST_F(DatasetTest, getGmlsServer) { // NOLINT
    const auto source = DatasetSource::createServer("23ku", Client::createClientForMockServer());
    const auto accessor = source.getAccessor();
    const auto gml_files = accessor->getGmlFiles(PredefinedCityModelPackage::Building | PredefinedCityModelPackage::LandUse);
    auto actual_gml_files = std::vector<std::string>();
    for(const auto& gml_file : *gml_files){
        actual_gml_files.push_back(gml_file.getPath());
    }
    const std::vector<std::string> expected_files = {
            u8"https://plateau-api-mock-v2.deta.dev/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml",
            u8"https://plateau-api-mock-v2.deta.dev/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392670_bldg_6697_2_op.gml",
            u8"https://plateau-api-mock-v2.deta.dev/13100_tokyo23-ku_2020_citygml_3_2_op/udx/luse/533926_luse_6668_2_op.gml",
            u8"https://plateau-api-mock-v2.deta.dev/13100_tokyo23-ku_2020_citygml_3_2_op/udx/luse/533926_luse_6668_2_op.gml",
            u8"https://plateau-api-mock-v2.deta.dev/13100_tokyo23-ku_2020_citygml_3_2_op/udx/luse/533926_luse_6697_park_op.gml"
    };
    checkVectors(expected_files, actual_gml_files);
}

TEST_F(DatasetTest, getAllMeshCodes) { // NOLINT
    const auto& mesh_codes = local_dataset_accessor->getMeshCodes();
    ASSERT_TRUE(mesh_codes.size() > 0);
}

namespace {
    void checkFilesExist(const std::vector<std::string>& file_relative_paths, const fs::path& base_path) {
        for (const auto& relative_path : file_relative_paths) {
            auto path = (fs::path(base_path) / fs::u8path(relative_path)).make_preferred();
            ASSERT_TRUE(fs::exists(path)) << path << " does not exist";
            ASSERT_TRUE(fs::file_size(path) > 0) << "downloaded file size is zero : " << path;
        }
    }
}

TEST_F(DatasetTest, fetch_local_generates_files) { // NOLINT
    // テスト用の一時的なフォルダを fetch のコピー先とし、そこにファイルが存在するかテストします。
    // パスに日本語を含むケースで動作確認します。
    auto temp_test_dir = fs::u8path(u8"../テスト用一時ディレクトリ");
    fs::remove_all(temp_test_dir);
    //    const auto& test_gml_info = local_dataset_accessor.getGmlFile(PredefinedCityModelPackage::Building, 0);
    const auto& test_gml_info = GmlFile(std::string(u8"../data/日本語パステスト/udx/bldg/53392642_bldg_6697_op2.gml"));
    test_gml_info.fetch(temp_test_dir.u8string());
    // gmlファイルがコピー先に存在します。
    auto bldg_dir = fs::path(temp_test_dir) / fs::u8path(u8"日本語パステスト/udx/bldg");
    auto gml_path = fs::path(bldg_dir).append(u8"53392642_bldg_6697_op2.gml").make_preferred();
    //    auto gml_path = fs::path(bldg_dir).append("53392587_bldg_6697_2_op.gml").make_preferred();
    ASSERT_TRUE(fs::exists(gml_path)) << gml_path << "does not exist.";

    // codelistsファイルがコピー先に存在します。
    auto codelists_dir = fs::path(temp_test_dir) / fs::u8path(u8"日本語パステスト/codelists");
    std::vector<std::string> codelists = {
            "Common_districtsAndZonesType.xml",
            "Common_prefecture.xml",
            "Common_localPublicAuthorities.xml",
            "extendedAttribute_key.xml",
            "extendedAttribute_key2.xml",
            "extendedAttribute_key106.xml",
            "Common_prefecture.xml"
    };
    checkFilesExist(codelists, codelists_dir);

    // 画像ファイルがコピー先に存在します。
    auto image_dir = fs::path(bldg_dir).append("53392642_bldg_6697_appearance");
    std::vector<std::string> images = {
            "hnap0876.tif",
            "hnap0878.tif",
            "hnap0285.tif",
            "hnap0276.tif",
            "hnap0275.tif",
            "hnap0034.tif",
            "hnap0286.tif",
            "hnap0279.tif"

    };
    checkFilesExist(images, image_dir);

//    fs::remove_all(temp_test_dir);
}

TEST_F(DatasetTest, fetch_server_generates_files) { // NOLINT
    // テスト用の一時的なフォルダを fetch のコピー先とし、そこにファイルが存在するかテストします。
    // パスに日本語を含むケースで動作確認します。
    auto temp_test_dir = fs::u8path(u8"../テスト用一時ディレクトリ");
    fs::remove_all(temp_test_dir);
    const auto file_url = std::string(Client::getMockServerUrl() + "/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml");
    const auto client = Client::createClientForMockServer();
    const auto& test_gml_info = GmlFile(file_url, client);
    test_gml_info.fetch(temp_test_dir.u8string());
    // gmlファイルがコピー先に存在します。
    auto bldg_dir = fs::path(temp_test_dir).append(u8"13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg");
    auto gml_path = fs::path(bldg_dir).append(u8"53392642_bldg_6697_2_op.gml").make_preferred();
//        auto gml_path = fs::path(bldg_dir).append("53392587_bldg_6697_2_op.gml").make_preferred();
    ASSERT_TRUE(fs::exists(gml_path)) << fs::absolute(gml_path) << " does not exist.";
    ASSERT_TRUE(fs::file_size(gml_path) > 0) << "gml file is not empty.";

    // codelistsファイルがコピー先に存在します。
    auto codelists_dir = fs::path(temp_test_dir).append(u8"13100_tokyo23-ku_2020_citygml_3_2_op/codelists");
    std::vector<std::string> codelists = {
            "Common_districtsAndZonesType.xml",
            "Common_prefecture.xml",
            "Common_localPublicAuthorities.xml",
            "extendedAttribute_key.xml",
            "extendedAttribute_key2.xml",
            "extendedAttribute_key106.xml",
    };
    checkFilesExist(codelists, codelists_dir);

    // 画像ファイルがコピー先に存在します。
    auto image_dir = fs::path(bldg_dir).append("53392642_bldg_6697_appearance");
    std::vector<std::string> images = {
            "hnap0876.jpg",
            "hnap0878.jpg",
            "hnap0285.jpg",
            "hnap0276.jpg",
            "hnap0275.jpg",
            "hnap0034.jpg",
            "hnap0286.jpg",
            "hnap0279.jpg"

    };
    checkFilesExist(images, image_dir);

    fs::remove_all(temp_test_dir);
}

namespace { // テスト filterByMeshCodes で使う無名名前空間の関数です。
    bool doResultOfFilterByMeshCodesContainsMeshCode(const std::string& mesh_code_str,
                                                     const IDatasetAccessor& udx_file_collection,
                                                     const PredefinedCityModelPackage sub_folder) {
        auto mesh_code = std::vector<MeshCode>{ MeshCode(mesh_code_str) };
        auto filtered_collection = udx_file_collection.filterByMeshCodes(mesh_code);
        auto gml_vector = filtered_collection->getGmlFiles(sub_folder);
        bool contains_mesh_code = false;
        for (const auto& building_gml : *gml_vector) {
            if (building_gml.getPath().find(mesh_code[0].get()) != std::string::npos) {
                contains_mesh_code = true;
            }
        }
        return contains_mesh_code;
    }
} // テスト filterByMeshCodes で使う無名名前空間の関数です。

TEST_F(DatasetTest, filter_by_mesh_codes) { // NOLINT
    ASSERT_TRUE(doResultOfFilterByMeshCodesContainsMeshCode("53392642", *local_dataset_accessor,
        PredefinedCityModelPackage::Building));
    ASSERT_FALSE(doResultOfFilterByMeshCodesContainsMeshCode("99999999", *local_dataset_accessor,
        PredefinedCityModelPackage::Building));
}


TEST_F(DatasetTest, get_max_lod_local){
    auto gml_files = local_dataset_accessor->getGmlFiles(PredefinedCityModelPackage::Building);
    auto gml = gml_files->at(0);
    ASSERT_EQ(gml.getMaxLod(), 2);
}
