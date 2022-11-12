#include <gtest/gtest.h>
#include <plateau/udx/available_lod_searcher.h>
#include <filesystem>

namespace plateau::udx {
    namespace fs = std::filesystem;

    class AvailableLodSearcherTest : public ::testing::Test {
    protected:
        const std::string gml_path = u8"../data/udx/bldg/53392642_bldg_6697_op2.gml";
    };

    TEST_F(AvailableLodSearcherTest, test){ // NOLINT
        AvailableLodSearcher searcher;
        auto lod_flag = searcher.searchLodsInFile(fs::u8path(gml_path));
        ASSERT_EQ(lod_flag.getFlag(), (unsigned)0b111);
    }
}
