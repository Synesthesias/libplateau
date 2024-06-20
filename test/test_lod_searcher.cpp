#include <gtest/gtest.h>
#include <plateau/dataset/lod_searcher.h>
#include <fstream>
#include "plateau/dataset/city_model_package.h"
#include <filesystem>
#include <chrono>

namespace plateau::dataset {
    namespace fs = std::filesystem;
    namespace chrono = std::chrono;

    class LodSearcherTest : public ::testing::Test {
    protected:
        void SetUp() override {
        }
    };

    namespace {
        int getMaxLod(const std::string& content) {
            auto string_buf = std::stringbuf(content);
            auto istream = std::istream(&string_buf);
            return LodSearcher::searchMaxLodInIstream(istream, CityModelPackageInfo::getPredefined(PredefinedCityModelPackage::Building).maxLOD());
        }
    }

    TEST_F(LodSearcherTest, SearchLodReturnsNumAfterLodColon) { // NOLINT
        ASSERT_EQ(getMaxLod(":lod2"), 2);
    }

    TEST_F(LodSearcherTest, WhenBodyIsEmptyReturnsMinus1) { // NOLINT
        ASSERT_EQ(getMaxLod(""), -1);
    }

    TEST_F(LodSearcherTest, WhenBodyIsOneSpaceReturnsMinus1) { // NOLINT
        ASSERT_EQ(getMaxLod(" "), -1);
    }

    TEST_F(LodSearcherTest, WhenLodIsMissingReturnsMinus1) { // NOLINT
        ASSERT_EQ(getMaxLod(":lod_is_not_found"), -1);
    }

    TEST_F(LodSearcherTest, MultipleLods) { // NOLINT
        ASSERT_EQ(getMaxLod(":lod2:lod3"), 3);
    }


    /// 注意：下のコメントアウト部分は削除しないでください。

    /// 今後、LODSearcher に変更を加えたとき、結果が変わらないかどうかを検証する手段として下のコードをコメントアウトして残しておきます。
    /// 実行方法:
    /// コメントアウトを外し、パスをテストしたいフォルダパスに変更してテスト実行します。
    /// するとGMLファイル名とLODが出力されるので、変更前後でそのdiffをとって出力が同じであることを検証できます。
//    TEST_F(LodSearcherTest, DisplayLodsRecursive) { // NOLINT
//        // 下のパスを、ご自分のPCでテストしたいフォルダのパスに変更してください。
//        const auto src_dir = fs::u8path(u8"F:\\Desktop\\13100_tokyo23-ku_2022_citygml_1_2_op\\udx");
//
//        std::cout << "==== LOD Search Result ====" << std::endl;
//        for(const auto& entry : fs::recursive_directory_iterator(src_dir)  ) {
//            if(entry.is_directory()) continue;
//            if(entry.path().extension() != ".gml") continue;
//            int lod = LodSearcher::searchMaxLodInFile(entry.path(), 999);
//            std::cout << entry.path().filename() << ": " << lod << std::endl;
//        }
//        std::cout << "==== LOD Search End ====" << std::endl;
//    }
}

