#include <gtest/gtest.h>
#include <plateau/udx/lod_searcher.h>
#include <filesystem>

namespace plateau::udx {
    namespace fs = std::filesystem;

    class LodSearcherTest : public ::testing::Test {
    protected:
//        const std::string gml_path = u8"../data/udx/bldg/53392642_bldg_6697_op2.gml";
        const std::string gml_path = u8"../data/udx/bldg/53393690_bldg_6697_2_op.gml";
    };

    TEST_F(LodSearcherTest, searchLodsInFile_returns_gml_lods){ // NOLINT
        auto lod_flag = LodSearcher::searchLodsInFile(fs::u8path(gml_path));
        ASSERT_EQ(lod_flag.getFlag(), (unsigned)0b111);
    }
}
