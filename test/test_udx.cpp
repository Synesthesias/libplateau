#include <filesystem>
#include <gtest/gtest.h>

#include <citygml/citygml.h>

#include <plateau/udx/udx_file_collection.h>

using namespace citygml;
using namespace plateau::udx;
namespace fs = std::filesystem;

class UdxTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        source_path_ = "../data";
        UdxFileCollection::find(source_path_, udx_file_collection_);
    }

    static void checkFiles(const std::vector<std::string>& expected, const std::vector<GmlFileInfo>& actual) {
        std::vector<std::string> actual_file_names;
        std::transform(actual.begin(), actual.end(), std::back_inserter(actual_file_names),
            [](const GmlFileInfo& file_info) { return file_info.getPath(); });
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
    UdxFileCollection udx_file_collection_;
};

TEST_F(UdxTest, getAllGmls) {
    const std::vector expected_bldg_files =
    { std::filesystem::path("../data/udx/bldg/53392642_bldg_6697_op2.gml").make_preferred().string() };
    std::vector<std::string> actual_files;

    checkVectors(expected_bldg_files, *udx_file_collection_.getGmlFiles(PredefinedCityModelPackage::Building));
}

TEST_F(UdxTest, getAllMeshCodes) {
    const auto mesh_codes = udx_file_collection_.getMeshCodes();
    ASSERT_EQ(mesh_codes.size(), 1);
}

namespace{
    void checkFilesExist(const std::vector<std::string>& file_relative_paths, const fs::path& base_path){
        for(const auto& relative_path : file_relative_paths){
            auto path = fs::path(base_path).append(relative_path).make_preferred();
            ASSERT_TRUE(fs::exists(path));
        }
    }
}

TEST_F(UdxTest, fetch_generates_files){
    // テスト用の一時的なフォルダを fetch のコピー先とし、そこにファイルが存在するかテストします。
    auto temp_test_dir = std::filesystem::path("../temp_test_dir").string();
    fs::remove_all(temp_test_dir);
//    const auto& test_gml_info = udx_file_collection_.getGmlFileInfo(PredefinedCityModelPackage::Building, 0);
    const auto& test_gml_info = GmlFileInfo("../data/udx/bldg/53392642_bldg_6697_op2.gml");
    udx_file_collection_.fetch(temp_test_dir, test_gml_info);
    // gmlファイルがコピー先に存在します。
    auto bldg_dir = fs::path(temp_test_dir).append("data/udx/bldg");
    auto gml_path = fs::path(bldg_dir).append("53392642_bldg_6697_op2.gml").make_preferred();
//    auto gml_path = fs::path(bldg_dir).append("53392587_bldg_6697_2_op.gml").make_preferred();
    std::cout << gml_path << std::endl;
    ASSERT_TRUE(fs::exists(gml_path));

    // codelistsファイルがコピー先に存在します。
    auto codelists_dir = fs::path(temp_test_dir).append("data/codelists");
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

    fs::remove_all(temp_test_dir);
}

namespace { // テスト filterByMeshCodes で使う無名名前空間の関数です。
    bool doResultOfFilterByMeshCodesContainsMeshCode(const std::string& mesh_code_str,
                                                     const UdxFileCollection& udx_file_collection,
                                                     const PredefinedCityModelPackage sub_folder) {
        auto mesh_code = std::vector<MeshCode>{MeshCode(mesh_code_str)};
        auto filtered_collection = udx_file_collection.filterByMeshCodes(mesh_code);
        auto gml_vector = filtered_collection->getGmlFiles(sub_folder);
        bool contains_mesh_code = false;
        for (const auto& building_gml: *gml_vector) {
            if (building_gml.find(mesh_code[0].get()) != std::string::npos) {
                contains_mesh_code = true;
            }
        }
        return contains_mesh_code;
    }
} // テスト filterByMeshCodes で使う無名名前空間の関数です。

TEST_F(UdxTest, filter_by_mesh_codes) {
    ASSERT_TRUE(doResultOfFilterByMeshCodesContainsMeshCode("53392642", udx_file_collection_,
                                                            PredefinedCityModelPackage::Building));
    ASSERT_FALSE(doResultOfFilterByMeshCodesContainsMeshCode("99999999", udx_file_collection_,
                                                             PredefinedCityModelPackage::Building));
}

//TEST_F(UdxTest, getAllSubFolders) {
//    const auto sub_folders = udx_file_collection_.getSubFolders();
//    std::vector<std::string> sub_folder_names;
//    std::transform(sub_folders->begin(), sub_folders->end(), std::back_inserter(sub_folder_names),
//    [](const UdxSubFolder& sub_folder) { return sub_folder; });
//    const std::vector expected = {
//        UdxSubFolder::bldg,
//        UdxSubFolder::dem,
//        UdxSubFolder::luse,
//        UdxSubFolder::frn,
//        UdxSubFolder::lsld,
//        UdxSubFolder::tran,
//        UdxSubFolder::urf,
//        std::string("brid")
//    };
//    checkVectors(expected, sub_folder_names);
//}
//
//TEST_F(UdxTest, copyAllRelatedFiles) {
//    const auto destination = "test_dir";
//    udx_file_collection_.copyAllFiles(destination);
//    udx_file_collection_.copyCodelistFiles(destination);
//    EXPECT_TRUE(fs::exists(destination + std::string("/data/udx/bldg/53392642_bldg_6697_op2.gml")));
//    EXPECT_TRUE(fs::exists(destination + std::string("/data/udx/bldg/53392642_bldg_6697_appearance/hnap0276.tif")));
//    EXPECT_TRUE(fs::exists(destination + std::string("/data/udx/luse/533925_luse_6697_park_op.gml")));
//    fs::remove_all(destination);
//}
//
//TEST_F(UdxTest, copyAllRelatedFilesInSubFolder) {
//    const auto destination = "test_dir";
//    const auto result = udx_file_collection_.copyFiles(destination, UdxSubFolder("brid"));
//    EXPECT_FALSE(fs::exists(destination + std::string("/data/udx/bldg/53392642_bldg_6697_op2.gml")));
//    EXPECT_TRUE(fs::exists(destination + std::string("/data/udx/brid/53394525_brid_6697_op.gml")));
//    fs::remove_all(destination);
//}
//
//std::string WStringToString
//(
//    std::wstring oWString
//) {
//    // wstring → SJIS
//    int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str()
//        , -1, (char*)NULL, 0, NULL, NULL);
//
//    // バッファの取得
//    CHAR* cpMultiByte = new CHAR[iBufferSize];
//
//    // wstring → SJIS
//    WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1, cpMultiByte
//        , iBufferSize, NULL, NULL);
//
//    // stringの生成
//    std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);
//
//    // バッファの破棄
//    delete[] cpMultiByte;
//
//    // 変換結果を返す
//    return(oRet);
//}
//
//TEST_F(UdxTest, copyToFolderWithMultiByteName) {
//    // UTF8のstringを初期化するためのugly hack
//    const auto destination = fs::path("テスト").u8string();
//
//    std::cout << destination;
//    const auto result = udx_file_collection_.copyFiles(destination, UdxSubFolder("brid"));
//    fs::remove_all(fs::u8path(destination));
//}
//
//TEST_F(UdxTest, getMeshCode) {
//    const auto mesh_codes = udx_file_collection_.getMeshCodes();
//    std::vector<std::string> mesh_code_names;
//    std::transform(mesh_codes->begin(), mesh_codes->end(), std::back_inserter(mesh_code_names),
//    [](const MeshCode& mesh_code) { return mesh_code.get(); });
//
//    const std::vector expected = {
//        std::string("53392642"),
//        std::string("53394525"),
//        std::string("533925")
//    };
//
//    checkVectors(expected, mesh_code_names);
//}
//
//TEST_F(UdxTest, filter) {
//    const std::vector mesh_codes = {
//        MeshCode("53392642"),
//        MeshCode("533925")
//    };
//    auto filtered = UdxFileCollection::filter(udx_file_collection_, mesh_codes);
//
//    const auto files = filtered.getAllGmlFiles();
//
//    std::vector<std::string> expected = {
//        "../data/udx/bldg/53392642_bldg_6697_op2.gml",
//        "../data/udx/dem/533925_dem_6697_op.gml",
//        "../data/udx/lsld/533925_lsld_6668_op.gml",
//        "../data/udx/luse/533925_luse_6668_2_op.gml",
//        "../data/udx/luse/533925_luse_6697_park_op.gml",
//        "../data/udx/tran/533925_tran_6697_op.gml",
//        "../data/udx/urf/533925_urf_6668_boka_op.gml",
//        "../data/udx/urf/533925_urf_6668_kodo_op.gml",
//        "../data/udx/urf/533925_urf_6668_yoto_op.gml",
//    };
//    for (auto& file : expected) {
//        file = fs::path(file).make_preferred().string();
//    }
//    checkFiles(expected, *files);
//
//    filtered = UdxFileCollection::filter(udx_file_collection_, {});
//    auto codes = filtered.getMeshCodes();
//    std::cout << codes->size();
//
//    checkFiles(expected, *files);
//}
