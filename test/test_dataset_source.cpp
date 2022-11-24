#include <gtest/gtest.h>
#include <plateau/dataset/dataset_source.h>

namespace plateau::dataset {
    class DatasetSourceTest : public ::testing::Test {

    };

    TEST_F(DatasetSourceTest, get_accessor_of_local_source_returns_local_accessor) { // NOLINT
        auto source = DatasetSource("../data");
        auto accessor = source.getAccessor();
        auto mesh_code_str = accessor->getMeshCodes().at(0).get();
        ASSERT_EQ(mesh_code_str, "53392642");
    }
}
