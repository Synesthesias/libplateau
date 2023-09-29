
#include "test_granularity_converter.h"
#include "citygml/citygml.h"
#include <queue>
#include "../src/c_wrapper/granularity_converter_c.cpp"

using namespace plateau::granularityConvert;
using namespace plateau::polygonMesh;


TEST_F(GranularityConverterTest, convertAreaModel) { // NOLINT
    createTestModelOfArea().test();
}

namespace {
    std::shared_ptr<Model> loadTestGML() {
        auto parser_params = citygml::ParserParams();
        auto city_model = citygml::load(u8"../data/日本語パステスト/udx/bldg/53392642_bldg_6697_op2.gml", parser_params);
        auto extract_option = MeshExtractOptions();
        auto src_model = MeshExtractor::extract(*city_model, extract_option);
        return src_model;
    }
}

TEST_F(GranularityConverterTest, ConvertFromTestGmlFile) { // NOLINT
    auto src_model = loadTestGML();
    auto convert_option_area = GranularityConvertOption(MeshGranularity::PerCityModelArea, 10);
    auto convert_option_feature = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
    auto dst_model_area = GranularityConverter().convert(*src_model, convert_option_area);
    auto dst_model_feature = GranularityConverter().convert(*src_model, convert_option_feature);
}

TEST_F(GranularityConverterTest, CWrapperWorks) { // NOLINT
    auto src_model = loadTestGML();
    auto convert_option = GranularityConvertOption(MeshGranularity::PerCityModelArea, 10);
    Model* out_model_ptr;
    plateau_granularity_converter_convert(
            src_model.get(), &out_model_ptr, convert_option);
}

TEST_F(GranularityConverterTest, RootNodeAreaToAtomic) { // NOLINT
//    auto src_model = createTestModelInRoot();
//    auto option = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
//    auto dst_model = GranularityConverter().convert(src_model, option);
    // TODO ここでチェック
}


namespace {
    std::unique_ptr<Mesh> createTestMeshOfAreaGranularity() {
        TVec3d base_pos = {0, 0, 0};
        unsigned int base_id = 0;
        std::vector<TVec3d> vertices;
        std::vector<unsigned int> indices;
        std::vector<TVec2f> uv1;
        std::vector<TVec2f> uv4;
        std::vector<SubMesh> sub_meshes;

        const std::vector<CityObjectIndex> city_object_indices = {
                {0, -1}, {0,0}, {0,1},
                {1, 0}, {1, 1}, {1, -1}
        };

        // 四角ポリゴンを複数作ります。
        for(int i=0; i < city_object_indices.size(); i++) {
            vertices.insert(vertices.end(), {
                    base_pos,
                    base_pos + TVec3d{0, 1, 0},
                    base_pos + TVec3d{1, 1, 0},
                    base_pos + TVec3d{1, 0, 0}
            });

            std::vector<unsigned int> new_indices = {
                    base_id, base_id + 1, base_id + 2,
                    base_id, base_id + 2, base_id + 3
            };
            indices.insert(indices.end(), new_indices.begin(), new_indices.end());

            auto new_sub_mesh = SubMesh(indices.size() - new_indices.size(), indices.size()-1, "dummy_tex_path_" + std::to_string(i), nullptr);
            sub_meshes.push_back(new_sub_mesh);

            uv1.insert(uv1.end(), {
                    TVec2f{0, 0}, TVec2f{0, 1}, TVec2f{1, 1}, TVec2f{1, 0}
            });

            for(int j=0; j<4; j++) {
                auto city_obj_index = city_object_indices.at(i);
                uv4.emplace_back(city_obj_index.primary_index, city_obj_index.atomic_index);
            }

            base_pos = base_pos + TVec3d{2, 0, 0};
            base_id += 4;
        }

        auto mesh = std::make_unique<Mesh>(Mesh());
        mesh->addVerticesList(vertices);
        mesh->addIndicesList(indices, 0, false);
        mesh->setUV1(std::move(uv1));
        mesh->setUV4(std::move(uv4));
        mesh->setSubMeshes(sub_meshes);

        auto city_objs = CityObjectList();
        city_objs.add({0,-1}, "primary-0");
        city_objs.add({0, 0}, "atomic-0-0");
        city_objs.add({0, 1}, "atomic-0-1");
        city_objs.add({1, -1}, "primary-1");
        city_objs.add({1, 0}, "atomic-1-0");
        city_objs.add({1, 1}, "atomic-1-1");
        mesh->setCityObjectList(city_objs);
        return mesh;
    }

    ModelForTest::TGranularityToExpect createExpectsForTestMeshArea() {
        auto atomic_expect = ModelExpect(std::vector<std::string>{
                                                 "gml_node",
                                                 "lod_node",
                                                 "primary-0",
                                                 "primary-1",
                                                 "atomic-0-0",
                                                 "atomic-0-1",
                                                 "atomic-1-0",
                                                 "atomic-1-1"
                                         },
                                         std::vector<bool>{false, false, true, true, true, true, true, true},
                // 各ノードの頂点数は4です。（ルート除く）
                                         std::vector<int>{0, 0, 4, 4, 4, 4, 4, 4},
                                         std::vector<std::set<CityObjectIndex>>{
                                                 // 各ノードのCityObjectIndexは(0,0)です。
                                                 std::set<CityObjectIndex>{
                                                         CityObjectIndex{-999, -999}}, // メッシュを持たないモードはこの値は無視されるので何でもよいです
                                                 std::set<CityObjectIndex>{CityObjectIndex{-999, -999}},
                                                 std::set<CityObjectIndex>{CityObjectIndex{0, 0}},
                                                 std::set<CityObjectIndex>{CityObjectIndex{0, 0}},
                                                 std::set<CityObjectIndex>{CityObjectIndex{0, 0}},
                                                 std::set<CityObjectIndex>{CityObjectIndex{0, 0}},
                                                 std::set<CityObjectIndex>{CityObjectIndex{0, 0}},
                                                 std::set<CityObjectIndex>{CityObjectIndex{0, 0}}
                                         },
                                         std::vector<CityObjectList>{
                                                 CityObjectList(),
                                                 CityObjectList(),
                                                 CityObjectList{{{CityObjectIndex{0, 0}, "primary-0"}}},
                                                 CityObjectList{{{CityObjectIndex{0, 0}, "primary-1"}}},
                                                 CityObjectList{{{CityObjectIndex{0, 0}, "atomic-0-0"}}},
                                                 CityObjectList{{{CityObjectIndex{0, 0}, "atomic-0-1"}}},
                                                 CityObjectList{{{CityObjectIndex{0, 0}, "atomic-1-0"}}},
                                                 CityObjectList{{{CityObjectIndex{0, 0}, "atomic-1-1"}}}
                                         },
                                         std::vector<std::vector<SubMesh>>{
                                                 {},
                                                 {},
                                                 {SubMesh(0, 5, "dummy_tex_path_0", nullptr)},
                                                 {SubMesh(0, 5, "dummy_tex_path_5", nullptr)},
                                                 {SubMesh(0, 5, "dummy_tex_path_1", nullptr)},
                                                 {SubMesh(0, 5, "dummy_tex_path_2", nullptr)},
                                                 {SubMesh(0, 5, "dummy_tex_path_3", nullptr)},
                                                 {SubMesh(0, 5, "dummy_tex_path_4", nullptr)},
                                         });

        auto primary_expect = ModelExpect(
                std::vector<std::string>{"gml_node", "lod_node", "primary-0", "primary-1"},
                std::vector<bool>{false, false, true, true},
                std::vector<int>{0, 0, 4 * 3, 4 * 3},
                std::vector<std::set<CityObjectIndex>>{
                        std::set<CityObjectIndex>(),
                        std::set<CityObjectIndex>(),
                        std::set<CityObjectIndex>{{{0, -1}, {0, 0}, {0, 1}}
                        },
                        std::set<CityObjectIndex>{{{0, -1}, {0, 0}, {0, 1}}}
                },
                std::vector<CityObjectList>{
                        CityObjectList(),
                        CityObjectList(),
                        CityObjectList{{{
                                                {CityObjectIndex{0, -1}, "primary-0"},
                                                {CityObjectIndex{0, 0}, "atomic-0-0"},
                                                {CityObjectIndex{0, 1}, "atomic-0-1"}
                                        }}},
                        CityObjectList{{{
                                                {CityObjectIndex{0, -1}, "primary-1"},
                                                {CityObjectIndex{0, 0}, "atomic-1-0"},
                                                {CityObjectIndex{0, 1}, "atomic-1-1"}
                                        }}}
                },
                std::vector<std::vector<SubMesh>>{
                        {},
                        {},
                        {
                                SubMesh(0, 5, "dummy_tex_path_0", nullptr),
                                SubMesh(6, 11, "dummy_tex_path_1", nullptr),
                                SubMesh(12, 17, "dummy_tex_path_2", nullptr)
                        },
                        {
                                SubMesh(0, 5, "dummy_tex_path_5", nullptr),
                                SubMesh(6, 11, "dummy_tex_path_3", nullptr),
                                SubMesh(12, 17, "dummy_tex_path_4", nullptr)
                        }
                }
        );
        auto area_expect = ModelExpect(
                std::vector<std::string>{"combined"},
                std::vector<bool>{true},
                // 頂点数は四角形ポリゴンを複数マージしたものです。
                std::vector<int>{4 * 6},
                std::vector<std::set<CityObjectIndex>>{
                        // 1つのメッシュの中に6通りのCityObjectIndexが含まれます。
                        std::set<CityObjectIndex>{
                                CityObjectIndex{0, -1},
                                CityObjectIndex{0, 0},
                                CityObjectIndex{0, 1},
                                CityObjectIndex{1, -1},
                                CityObjectIndex{1, 0},
                                CityObjectIndex{1, 1}
                        }
                },
                std::vector<CityObjectList>{
                        CityObjectList{{
                                               {CityObjectIndex{0, -1}, "primary-0"},
                                               {CityObjectIndex{0, 0}, "atomic-0-0"},
                                               {CityObjectIndex{0, 1}, "atomic-0-1"},
                                               {CityObjectIndex{1, -1}, "primary-1"},
                                               {CityObjectIndex{1, 0}, "atomic-1-0"},
                                               {CityObjectIndex{1, 1}, "atomic-1-1"}
                                       }}
                },
                std::vector<std::vector<SubMesh>>{
                        {
                                SubMesh(0, 5, "dummy_tex_path_0", nullptr),
                                SubMesh(6, 11, "dummy_tex_path_1", nullptr),
                                SubMesh(12, 17, "dummy_tex_path_2", nullptr),
                                SubMesh(18, 23, "dummy_tex_path_5", nullptr),
                                SubMesh(24, 29, "dummy_tex_path_3", nullptr),
                                SubMesh(30, 35, "dummy_tex_path_4", nullptr)
                        }
                }
        );

        auto convert_expects = ModelForTest::TGranularityToExpect{
                {MeshGranularity::PerAtomicFeatureObject, atomic_expect},
                {MeshGranularity::PerPrimaryFeatureObject, primary_expect},
                {MeshGranularity::PerCityModelArea, area_expect}
        };
        return convert_expects;
    }
}

ModelForTest GranularityConverterTest::createTestModelOfArea() {
    auto mesh = createTestMeshOfAreaGranularity();

    auto model = Model();
    auto& gml_node = model.addNode(Node("gml_node"));
    auto& lod_node = gml_node.addChildNode(Node("lod_node"));
    auto& mesh_node = lod_node.addChildNode(Node("mesh_node"));
    mesh_node.setMesh(std::move(mesh));
    auto expects = createExpectsForTestMeshArea();
    auto ret = ModelForTest(std::move(model), expects);
    return ret;
}

//ModelForTest createTestModelOfAreaOnlyRoot() {
//    auto mesh = createTestMeshOfAreaGranularity();
//    auto expects = createExpectsForTestMeshArea();
//    auto model = Model();
//    model.addNode(Node("root_node"));
//}
