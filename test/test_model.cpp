
#include "gtest/gtest.h"
#include "plateau/polygon_mesh/mesh_extractor.h"
#include "citygml/citymodel.h"
#include "citygml/citygml.h"

namespace plateau::polygonMesh {
    using namespace citygml;

    class ModelTest : public ::testing::Test {
    protected:
        const std::shared_ptr<const CityModel> city_model_ = load("../data/日本語パステスト/udx/bldg/53392642_bldg_6697_op2.gml",
                                                                  ParserParams());
    };

    TEST_F(ModelTest, when_mesh_extract_then_empty_nodes_are_removed) {
        auto extract_options = MeshExtractOptions();
        extract_options.mesh_granularity = MeshGranularity::PerCityModelArea;
        extract_options.grid_count_of_side = 3;
        extract_options.max_lod = 3;
        auto model = MeshExtractor::extract(*city_model_, extract_options);

        // 今回のテストデータに関しては、
        // eraseEmptyNodes() を実行しなければ LOD0 ～ LOD3 のノードが生成されます。
        // このうちポリゴンを含まないのは LOD3 です。
        // eraseEmptyNode() が実行されていれば、 LOD3 のノードが消えるはずなのでこれを確認します。
        ASSERT_EQ(model->getRootNodeCount(), 3);
        ASSERT_EQ(model->getRootNodeAt(0).getName(), "LOD0");
        ASSERT_EQ(model->getRootNodeAt(1).getName(), "LOD1");
        ASSERT_EQ(model->getRootNodeAt(2).getName(), "LOD2");
    }
}
