
#include "test_granularity_converter.h"
#include "citygml/citygml.h"
#include <queue>
#include "../src/c_wrapper/granularity_converter_c.cpp"

using namespace plateau::granularityConvert;
using namespace plateau::polygonMesh;


TEST_F(GranularityConverterTest, convertAreaToAtomic) { // NOLINT
    createTestModelOfArea().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToPrimary) { // NOLINT
    createTestModelOfArea().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaToArea) { // NOLINT
    createTestModelOfArea().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAreaRootToAtomic) { // NOLINT
    createTestModelOfArea_OnlyRoot().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaRootToPrimaruy) { // NOLINT
    createTestModelOfArea_OnlyRoot().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAreaRootToArea) { // NOLINT
    createTestModelOfArea_OnlyRoot().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToArea) { // NOLINT
    createTestModelOfAtomic().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertAtomicToPrimary) { // NOLINT
    createTestModelOfAtomic().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertAtomicToAtomic) { // NOLINT
    createTestModelOfAtomic().test(MeshGranularity::PerAtomicFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToArea) { // NOLINT
    createTestModelOfAtomic().test(MeshGranularity::PerCityModelArea);
}

TEST_F(GranularityConverterTest, convertPrimaryToPrimary) { // NOLINT
    createTestModelOfAtomic().test(MeshGranularity::PerPrimaryFeatureObject);
}

TEST_F(GranularityConverterTest, convertPrimaryToAtomic) { // NOLINT
    createTestModelOfAtomic().test(MeshGranularity::PerAtomicFeatureObject);
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
    /// 地域単位のテスト用メッシュを作ります。
    std::unique_ptr<Mesh> createTestMeshOfArea() {
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

    /// 地域単位のモデルを変換したときにどうなるかをここに記述します。
    ModelForTest::TGranularityToExpect createExpectsForTestMeshArea() {
        // メッシュを持たないオブジェクトはCityObjectIndexは無視されるので何の値でも良いです。
        const CityObjectIndex none_coi = {-999, -999};

        // 最小地物単位の場合
        auto atomic_expect = ModelExpect(
                std::vector<NodeExpect>{
                    NodeExpect("gml_node", false, 0, {{none_coi}}, {}, {}),
                    NodeExpect("lod_node", false, 0, {{none_coi}}, {}, {}),
                    NodeExpect("primary-0", true, 4, {{{0, 0}}}, {{{{0,0}, "primary-0"}}}, {SubMesh(0,5,"dummy_tex_path_0",nullptr)}),
                    NodeExpect("primary-1", true, 4, {{{0, 0}}}, {{{{0,0}, "primary-1"}}}, {SubMesh(0,5,"dummy_tex_path_5",nullptr)}),
                    NodeExpect("atomic-0-0", true, 4, {{{0, 0}}}, {{{{0,0}, "atomic-0-0"}}}, {SubMesh(0,5,"dummy_tex_path_1",nullptr)}),
                    NodeExpect("atomic-0-1", true, 4, {{{0, 0}}}, {{{{0,0}, "atomic-0-1"}}}, {SubMesh(0,5,"dummy_tex_path_2",nullptr)}),
                    NodeExpect("atomic-1-0", true, 4, {{{0, 0}}}, {{{{0,0}, "atomic-1-0"}}}, {SubMesh(0,5,"dummy_tex_path_3",nullptr)}),
                    NodeExpect("atomic-1-1", true, 4, {{{0, 0}}}, {{{{0,0}, "atomic-1-1"}}}, {SubMesh(0,5,"dummy_tex_path_4",nullptr)})
                });

        // 主要地物の場合
        auto primary_expect = ModelExpect(
                std::vector<NodeExpect>{
                    NodeExpect("gml_node", false, 0, {{}}, {}, {}),
                    NodeExpect("lod_node", false, 0, {{}}, {}, {}),
                    NodeExpect("primary-0", true, 4*3, {{{0,-1}, {0,0}, {0,1}}}, {{{{0,-1}, "primary-0"}, {{0,0}, "atomic-0-0"}, {{0,1},"atomic-0-1"}}},
                               {SubMesh(0,5,"dummy_tex_path_0",nullptr),
                                SubMesh(6,11,"dummy_tex_path_1", nullptr),
                                SubMesh(12,17,"dummy_tex_path_2",nullptr)
                               }),
                    NodeExpect("primary-1", true, 4*3, {{{0,-1}, {0,0}, {0,1}}}, {{{{0,-1}, "primary-1"}, {{0,0}, "atomic-1-0"}, {{0,1},"atomic-1-1"}}},
                                {SubMesh(0,5,"dummy_tex_path_5",nullptr),
                                 SubMesh(6,11,"dummy_tex_path_3", nullptr),
                                 SubMesh(12,17,"dummy_tex_path_4",nullptr)}
                                )
                });

        // 地域単位の場合
        auto area_expect = ModelExpect(
                std::vector<NodeExpect>{
                        NodeExpect("gml_node", true, 4 * 6,
                                   {{
                                            {0, -1},
                                            {0, 0},
                                            {0, 1},
                                            {1, -1},
                                            {1, 0},
                                            {1, 1}
                                    }},
                                   {{
                                            {{0, -1}, "primary-0"},
                                            {{0, 0}, "atomic-0-0"},
                                            {{0, 1}, "atomic-0-1"},
                                            {{1, -1}, "primary-1"},
                                            {{1, 0}, "atomic-1-0"},
                                            {{1, 1}, "atomic-1-1"}
                                    }},
                                   {
                                           SubMesh(0, 5, "dummy_tex_path_0", nullptr),
                                           SubMesh(6, 11, "dummy_tex_path_1", nullptr),
                                           SubMesh(12, 17, "dummy_tex_path_2", nullptr),
                                           SubMesh(18, 23, "dummy_tex_path_5", nullptr),
                                           SubMesh(24, 29, "dummy_tex_path_3", nullptr),
                                           SubMesh(30, 35, "dummy_tex_path_4", nullptr)
                                   }
                        )}
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
    auto mesh = createTestMeshOfArea();

    auto model = Model();
    auto& gml_node = model.addNode(Node("gml_node"));
    auto& lod_node = gml_node.addChildNode(Node("lod_node"));
    auto& mesh_node = lod_node.addChildNode(Node("mesh_node"));
    mesh_node.setMesh(std::move(mesh));
    auto expects = createExpectsForTestMeshArea();
    auto ret = ModelForTest(std::move(model), expects);
    return ret;
}

ModelForTest GranularityConverterTest::createTestModelOfAtomic() {
    // テスト用モデルを手打ちコードで作るのは非常に手間なので、
    // 地域単位から最小地物単位への変換がうまくいっていることを前提に、変換したものをテストモデルとします。
    // もし前提機能が壊れた場合、他のテストで検出できるはずです。
    // その代わり、デバッガでブレークポイントを貼りながら実行する時は変換が2回行われ、Atomicから各種単位への変換は2回目のほうであることに留意してください。
    auto area_model = createTestModelOfArea();
    auto option = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_model.getModel(), option);
    // 期待する変換結果は地域単位と同じです。
    auto atomic_expect = area_model.getExpects();
    return {std::move(atomic_model), atomic_expect};
}

ModelForTest GranularityConverterTest::createTestModelOfPrimary() {
    auto area_model = createTestModelOfArea();
    auto option = GranularityConvertOption(MeshGranularity::PerPrimaryFeatureObject, 10);
    auto primary_model = GranularityConverter().convert(area_model.getModel(), option);
    auto primary_expect = area_model.getExpects();
    return {std::move(primary_model), primary_expect};
}

/// 地域単位のテスト用モデルのうち、gmlノードとlodノードをなくして、ルートノードに直接メッシュがあるパターンをテストします。
ModelForTest GranularityConverterTest::createTestModelOfArea_OnlyRoot() {
    auto mesh = createTestMeshOfArea();

    // このメソッドに登場するsrc_modelは、すべて変換前のモデルです。
    // 変換後のモデルをデバッガで確認したい場合は、ModelForTest::testメソッド内にブレークポイントを貼ってください。
    auto src_model = Model();

    auto& root_node = src_model.addNode(Node("root_node"));
    root_node.setMesh(std::move(mesh));

    // テストの期待値からgmlノードとlodノードの分を除いて、ルートノードを追加します。
    auto expects = createExpectsForTestMeshArea();

    auto& expects_area = expects.at(MeshGranularity::PerCityModelArea);
    expects_area.at(0).expect_node_name_ = "root_node";

    auto& expects_atomic = expects.at(MeshGranularity::PerAtomicFeatureObject);
    expects_atomic.eraseRange(0,1);
    expects_atomic.at(0).expect_node_name_ = "root_node";

    auto& expects_feature = expects.at(MeshGranularity::PerPrimaryFeatureObject);
    expects_feature.eraseRange(0, 1);
    expects_feature.at(0).expect_node_name_ = "root_node";

    auto ret = ModelForTest(std::move(src_model), expects);

    return ret;
}
