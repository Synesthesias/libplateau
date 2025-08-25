#include "gtest/gtest.h"
#include "citygml/citymodel.h"
#include "citygml/citygml.h"
#include <plateau/polygon_mesh/tile_extractor.h>

using namespace citygml;
using namespace plateau::geometry;

namespace plateau::polygonMesh {

    class TileExtractorTest : public ::testing::Test {
    protected:
        void SetUp() override {
            params_.tesselate = true;
        }
        // テストで使う共通パーツです。
        ParserParams params_;
    };

    TEST_F(TileExtractorTest, extract_with_grid) { // NOLINT

        const std::string gml_path = u8"../data/日本語パステスト/tile_test/tran/52353549_tran_6697_op.gml";
        const std::shared_ptr<const CityModel> city_model = load(gml_path, params_);
        ASSERT_TRUE(city_model);

        // zoomLevel 11の場合のグリッド分割
        MeshExtractOptions mesh_extract_options = MeshExtractOptions();
        mesh_extract_options.min_lod = 0;
        mesh_extract_options.max_lod = 4;
        mesh_extract_options.mesh_granularity = MeshGranularity::PerCityModelArea;
        mesh_extract_options.highest_lod_only = true;
        mesh_extract_options.grid_count_of_side = 2;

        const auto min = city_model->getEnvelope().getLowerBound();
        const auto max = city_model->getEnvelope().getUpperBound();

        std::vector<Extent> extents = {
            Extent(GeoCoordinate(min.x, min.y, min.z), GeoCoordinate(max.x, max.y, max.z))
        };

        const auto model = TileExtractor::extractWithGrid(*city_model, mesh_extract_options, extents);

		ASSERT_EQ(model->getRootNodeCount(), 4); // 2x2のグリッドなので最大4つのGRIDノードがあるはず（オブジェクトがなければ少なくなる）Test用の読込GMLは4分割される

		// グリッドノードの名前を確認
        const auto& grid_node = model->getRootNodeAt(0);
        const auto& grid_name = grid_node.getName();
        ASSERT_EQ(grid_name.substr(0, 4), "GRID");

        for (size_t i = 0; i < model->getRootNodeCount(); ++i) {
            const auto& node = model->getRootNodeAt(i);
            const auto& name = node.getName();
            ASSERT_EQ(name.substr(0, 4), "GRID");
        }

		// LODノードの名前を確認
        ASSERT_GT(grid_node.getChildCount(), 0);
        const auto& lod_node = grid_node.getChildAt(0);
        const auto& lod_name = lod_node.getName();
        ASSERT_EQ(lod_name.substr(0, 3), "LOD");

		// グリッドノードの子ノードの名前を確認
        ASSERT_GT(lod_node.getChildCount(), 0);
        const auto& first_model_node = lod_node.getChildAt(0);
        const auto& first_model_node_name = first_model_node.getName();
        ASSERT_EQ(first_model_node_name.substr(0, 5), "group");
    }

    TEST_F(TileExtractorTest, extract_with_combine) { // NOLINT

        // zoomLevel 9以下の場合の結合
        MeshExtractOptions mesh_extract_options = MeshExtractOptions();
        mesh_extract_options.min_lod = 0;
        mesh_extract_options.max_lod = 4;
        mesh_extract_options.mesh_granularity = MeshGranularity::PerCityModelArea;
        mesh_extract_options.highest_lod_only = true;
        mesh_extract_options.grid_count_of_side = 1;

        const std::string gml_path1 = u8"../data/日本語パステスト/tile_test/tran/52353548_tran_6697_op.gml";
        const std::string gml_path2 = u8"../data/日本語パステスト/tile_test/tran/52353549_tran_6697_op.gml";
        const std::shared_ptr<const CityModel> city_model1 = load(gml_path1, params_);
        const std::shared_ptr<const CityModel> city_model2 = load(gml_path2, params_);
        ASSERT_TRUE(city_model1);
        ASSERT_TRUE(city_model2);

        const auto min1 = city_model1->getEnvelope().getLowerBound();
        const auto max1 = city_model1->getEnvelope().getUpperBound();
        const auto min2 = city_model2->getEnvelope().getLowerBound();
        const auto max2 = city_model2->getEnvelope().getUpperBound();

        std::vector<Extent> extents = {
            Extent(GeoCoordinate(min1.x, min1.y, min1.z), GeoCoordinate(max1.x, max1.y, max1.z)),
            Extent(GeoCoordinate(min2.x, min2.y, min2.z), GeoCoordinate(max2.x, max2.y, max2.z)),
        };

		CityModelVector city_models = std::make_shared<std::vector<std::weak_ptr<const citygml::CityModel>>>();
        city_models->reserve(static_cast<size_t>(2));
        city_models->push_back(city_model1);
        city_models->push_back(city_model2);

        auto model = TileExtractor::extractWithCombine(city_models, mesh_extract_options, extents);

        // LODノードの名前を確認
        ASSERT_GT(model->getRootNodeCount(), 0);
        const auto& lod_node = model->getRootNodeAt(0);
        const auto& lod_name = lod_node.getName();
        ASSERT_EQ(lod_name.substr(0, 3), "LOD");

        ASSERT_EQ(lod_node.getChildCount(), 1); // 結合されるので子ノードは１つ

        // 子ノードの名前を確認
        const auto& first_model_node = lod_node.getChildAt(0);
        const auto& first_model_node_name = first_model_node.getName();
        ASSERT_EQ(first_model_node_name.substr(0, 5), "group");

    }
}
