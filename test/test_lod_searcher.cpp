#include <gtest/gtest.h>
#include <plateau/dataset/lod_searcher.h>
#include <fstream>

namespace plateau::dataset {
    class LodSearcherTest : public ::testing::Test {
    protected:
        void SetUp() override {
        }
    };

    namespace {
        int getMaxLod(const std::string& content) {
            auto string_buf = std::stringbuf(content);
            auto istream = std::istream(&string_buf);
            return LodSearcher::searchLodsInIstream(istream).getMax();
        }
    }

    TEST_F(LodSearcherTest, SearchLodReturnsNumAfterLodColon) { // NOLINT
        ASSERT_EQ(getMaxLod(":lod2"), 2);
    }

    TEST_F(LodSearcherTest, WhenBodyIsEmptyReturnsMinus1) { // NOLINT
        ASSERT_EQ(getMaxLod(""), -1);
    }

    TEST_F(LodSearcherTest, WhenLodIsMissingReturnsMinus1) { // NOLINT
        ASSERT_EQ(getMaxLod(":lod_is_not_found"), -1);
    }

    TEST_F(LodSearcherTest, MultipleLods) { // NOLINT
        ASSERT_EQ(getMaxLod(":lod2:lod3"), 3);
    }
}

