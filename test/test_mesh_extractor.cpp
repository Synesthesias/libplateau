#include "gtest/gtest.h"
#include "citygml/citymodel.h"
#include "citygml/citygml.h"
#include "../src/c_wrapper/mesh_extractor_c.cpp"
#include "../src/c_wrapper/model_c.cpp"
#include "../src/c_wrapper/city_model_c.cpp"
#include "../src/c_wrapper/citygml_c.cpp"
#include "../src/polygon_mesh/grid_merger.h"
#include <plateau/polygon_mesh/mesh_extractor.h>

using namespace citygml;
using namespace plateau::geometry;

namespace plateau::polygonMesh {

    class MeshExtractorTest : public ::testing::Test {
    protected:
        void SetUp() override {
            params_.tesselate = true;
            mesh_extract_options_.min_lod = 2;
            mesh_extract_options_.max_lod = 2;
            mesh_extract_options_.mesh_granularity = MeshGranularity::PerCityModelArea;
            mesh_extract_options_.grid_count_of_side = 5;
        }

        // テストで使う共通パーツです。
        ParserParams params_;
        const std::string gml_path_ = u8"../data/日本語パステスト/udx/bldg/53392642_bldg_6697_op2.gml";
        MeshExtractOptions mesh_extract_options_ = MeshExtractOptions();
        const std::shared_ptr<const CityModel> city_model_ = load(gml_path_, params_);
        void testExtractFromCWrapper() const;
        bool haveVertexRecursive(const Node& node) const;

        /**
         * MeshGranularity と LOD の全組み合わせをテストします。
         * @param check_func
         * 各組み合わせでのチェック内容を渡します。 引数にLODノード, LOD番号 をとる関数オブジェクトです。
         */
        void foreachMeshGranularityAndLOD(MeshExtractOptions options, std::function<void(Node&, unsigned)> check_func);
        void foreachMeshGranularity(MeshExtractOptions options, std::function<void(Model&)> check_func);
    };


    TEST_F(MeshExtractorTest, extract_returns_model_with_child_with_name) { // NOLINT
        auto model = MeshExtractor::extract(*city_model_, mesh_extract_options_);
        const auto& lod_node = model->getRootNodeAt(0);
        const auto& lod_name = lod_node.getName();
        ASSERT_EQ(lod_name, "LOD2");

        const auto& first_model_node = lod_node.getChildAt(0);
        const auto& first_model_node_name = first_model_node.getName();
        ASSERT_EQ(first_model_node_name, "group6");


    }

    TEST_F(MeshExtractorTest, extract_result_have_texture_url) { // NOLINT
        auto model = MeshExtractor::extract(*city_model_, mesh_extract_options_);
        const auto& lod_node = model->getRootNodeAt(0);
        // 存在するテクスチャURLを検索します。
        auto num_child = lod_node.getChildCount();
        int found_texture_num = 0;
        for (int i = 0; i < num_child; i++) {
            const auto& child = lod_node.getChildAt(i);
            const auto& mesh_opt = child.getMesh();
            if (!mesh_opt.has_value()) continue;
            const auto& sub_meshes = mesh_opt.value().getSubMeshes();
            if (sub_meshes.empty()) continue;
            for (auto& sub_mesh: sub_meshes) {
                const auto& tex_url = sub_mesh.getTexturePath();
                if (tex_url.empty()) continue;
                found_texture_num++;
            }
        }
        ASSERT_TRUE(found_texture_num > 5);
    }

    TEST_F(MeshExtractorTest, extract_can_exec_multiple_times) { // NOLINT
        for (int i = 0; i < 3; i++) {
            testExtractFromCWrapper();
        }
    }

    TEST_F(MeshExtractorTest,
           when_extract_atomic_building_then_primary_nodes_have_no_mesh_and_atomic_nodes_have_mesh) { // NOLINT
        MeshExtractOptions options = mesh_extract_options_;
        options.mesh_granularity = MeshGranularity::PerAtomicFeatureObject;
        auto model = MeshExtractor::extract(*city_model_, options);
        const auto& first_primary_node = model->getRootNodeAt(0).getChildAt(0);
        const auto& first_atomic_node = first_primary_node.getChildAt(0);
        ASSERT_FALSE(first_primary_node.getMesh().has_value());
        ASSERT_TRUE(first_atomic_node.getMesh().has_value());
    }

    TEST_F(MeshExtractorTest, extract_can_export_multiple_lods_when_granularity_is_per_city_model_area) { // NOLINT
        MeshExtractOptions options = mesh_extract_options_;
        options.mesh_granularity = MeshGranularity::PerCityModelArea;
        options.min_lod = 0;
        options.max_lod = 2;
        auto model = MeshExtractor::extract(*city_model_, options);
        const auto& lod0 = model->getRootNodeAt(0);
        const auto& lod1 = model->getRootNodeAt(1);
        const auto& lod2 = model->getRootNodeAt(2);
        ASSERT_EQ(lod0.getName(), "LOD0");
        ASSERT_EQ(lod1.getName(), "LOD1");
        ASSERT_EQ(lod2.getName(), "LOD2");
        auto lod0_grid1_num_vertices = lod0.getChildAt(1).getMesh().value().getVertices().size();
        auto lod1_grid1_num_vertices = lod1.getChildAt(1).getMesh().value().getVertices().size();
        auto lod2_grid1_num_vertices = lod2.getChildAt(1).getMesh().value().getVertices().size();
        ASSERT_GT(lod0_grid1_num_vertices, 0);
        ASSERT_GT(lod1_grid1_num_vertices, 0);
        ASSERT_GT(lod2_grid1_num_vertices, 0);
    }

    TEST_F(MeshExtractorTest, extract_can_export_multiple_lods_with_vertex) { // NOLINT

        foreachMeshGranularityAndLOD(
                mesh_extract_options_,
                [this](Node& lod_node, int lod_num) {
                    ASSERT_EQ(lod_node.getName(), "LOD" + std::to_string(lod_num));
                    ASSERT_TRUE(haveVertexRecursive(lod_node));
                }
        );
    }


    TEST_F(MeshExtractorTest, no_mesh_extracted_when_extent_range_is_zero_and_option_says_removing_outer_obj) { // NOLINT
        auto options = mesh_extract_options_;
        options.extent = Extent(GeoCoordinate(0, 0, 0), GeoCoordinate(0, 0, 0));
        foreachMeshGranularity(
                options,
                [](Model& model) {
                    ASSERT_EQ(model.getRootNodeCount(), 0);
                }
        );
    }

    TEST_F(MeshExtractorTest,
           no_mesh_extracted_when_extent_range_is_zero_and_option_says_removing_outer_polygons) { // NOLINT
        auto options = mesh_extract_options_;
        options.exclude_city_object_outside_extent = false;
        options.exclude_triangles_outside_extent = true;
        options.extent = Extent(GeoCoordinate(0, 0, 0), GeoCoordinate(0, 0, 0));
        foreachMeshGranularity(
                options,
                [](Model& model) {
                    ASSERT_EQ(model.getRootNodeCount(), 0);
                }
        );
    }

    TEST_F(MeshExtractorTest,
           some_vertices_extracted_when_extent_range_is_all_and_option_says_removing_outer_polygons) { // NOLINT
        auto options = mesh_extract_options_;
        options.exclude_city_object_outside_extent = false;
        options.exclude_triangles_outside_extent = true;
        options.extent = Extent::all();
        foreachMeshGranularityAndLOD(
                options,
                [this](Node& node, int lod_num) {
                    ASSERT_TRUE(haveVertexRecursive(node));
                }
        );
    }


    void MeshExtractorTest::testExtractFromCWrapper() const {

        const CityModelHandle* city_model_handle;
        plateau_load_citygml(gml_path_.c_str(), plateau_citygml_parser_params(), &city_model_handle,
                             DllLogLevel::LL_WARNING, nullptr, nullptr, nullptr);

        Model* model;
        plateau_create_model(&model);
        plateau_mesh_extractor_extract(city_model_handle, mesh_extract_options_, model);

        ASSERT_TRUE(model->getRootNodeCount() == 1);
        ASSERT_EQ(model->getRootNodeAt(0).getChildAt(0).getName(), "group6");
        plateau_delete_city_model(city_model_handle);
        plateau_delete_model(model);
    }

    bool MeshExtractorTest::haveVertexRecursive(const Node& node) const {
        const auto& mesh = node.getMesh();
        if (mesh.has_value() && !(mesh.value().getVertices().empty())) {
            return true;
        }
        auto num_child = node.getChildCount();
        for (int i = 0; i < num_child; i++) {
            if (haveVertexRecursive(node.getChildAt(i))) {
                return true;
            }
        }
        return false;
    }

    void MeshExtractorTest::foreachMeshGranularityAndLOD(MeshExtractOptions options,
                                                         std::function<void(Node&, unsigned)> check_func) {
        const std::vector<MeshGranularity> test_pattern_granularity = {MeshGranularity::PerCityModelArea,
                                                                       MeshGranularity::PerPrimaryFeatureObject,
                                                                       MeshGranularity::PerAtomicFeatureObject};

        options.min_lod = 0;
        options.max_lod = 2;
        foreachMeshGranularity(options, [&options, &check_func](Model& model) {
            for (unsigned lod = options.min_lod; lod <= options.max_lod; lod++) {
                auto& lod_node = model.getRootNodeAt(lod);
                check_func(lod_node, lod);
            }
        });
    }

    void MeshExtractorTest::foreachMeshGranularity(MeshExtractOptions options, std::function<void(Model&)> check_func) {
        const std::vector<MeshGranularity> test_pattern_granularity = {MeshGranularity::PerCityModelArea,
                                                                       MeshGranularity::PerPrimaryFeatureObject,
                                                                       MeshGranularity::PerAtomicFeatureObject};
        for (const auto granularity: test_pattern_granularity) {
            std::cout << "testing mesh_granularity = " << (int) granularity << std::endl;
            options.mesh_granularity = granularity;
            auto model = MeshExtractor::extract(*city_model_, options);
            check_func(*model);
        }
    }
}
