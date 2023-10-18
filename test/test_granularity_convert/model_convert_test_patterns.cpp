#include "gtest/gtest.h"
#include "model_convert_test_patterns.h"
#include <plateau/polygon_mesh/model.h>
#include <plateau/granularity_convert/granularity_converter.h>
#include <plateau/granularity_convert/node_queue.h>
#include <cassert>

using namespace plateau::polygonMesh;
using namespace plateau::granularityConvert;

void NodeExpect::checkNode(const plateau::polygonMesh::Node* node) const {
    EXPECT_EQ(node->getName(), expect_node_name_);
    EXPECT_EQ(!(node->getMesh() == nullptr), expect_have_mesh_);
    if(node->getMesh() != nullptr) {
        const auto mesh = node->getMesh();
        const auto vertex_count = mesh->getVertices().size();
        EXPECT_EQ(vertex_count, expect_vertex_count_);
        std::set<CityObjectIndex> obj_indices_in_mesh;
        for(int i=0; i<vertex_count; i++){
            const auto city_obj_id = CityObjectIndex::fromUV(mesh->getUV4().at(i));
            obj_indices_in_mesh.insert(city_obj_id);
        }
        std::cout << node->getName() << std::endl;
        EXPECT_EQ(obj_indices_in_mesh, expect_city_obj_id_set_);
        EXPECT_EQ(mesh->getCityObjectList(), expect_city_obj_list_);

        // SubMeshの一致を確認します。
        const auto& sub_meshes = mesh->getSubMeshes();
        const auto& expect_sub_meshes = expect_sub_meshes_;
        EXPECT_EQ(sub_meshes.size(), expect_sub_meshes.size());
        for(int i=0; i<sub_meshes.size(); i++) {
            EXPECT_EQ(sub_meshes.at(i), expect_sub_meshes.at(i));
        }

        EXPECT_EQ(mesh->getUV4().size(), mesh->getVertices().size()) << "UV4 Exists";
    }
}

/// 引数で与えられたModelの各Nodeを幅優先探索の順番で調べ、引数の各vector.at(index)と一致するかどうか調べます。
void checkModelBFS(const Model& model, const ModelExpect& expect) {

    std::queue<const plateau::polygonMesh::Node*> queue;

    for(int i=0; i<model.getRootNodeCount(); i++){
        queue.push(&model.getRootNodeAt(i));
    }

    int node_index = 0;
    for(; !queue.empty(); node_index++){
        const auto node = queue.front();
        queue.pop();

        expect.at(node_index).checkNode(node);


        for(int i=0; i<node->getChildCount(); i++){
            queue.push(&node->getChildAt(i));
        }

    }
    EXPECT_EQ(node_index, expect.nodeCount());
}

void ModelConvertTestPatterns::test(MeshGranularity granularity) {
    std::cout << "====== Model Test Start ======"<< std::endl;
    std::cout << "rootNodeCount: " << src_model_.getRootNodeCount() << std::endl;
        std::string granularity_str;
        switch(granularity) {
            case MeshGranularity::PerCityModelArea:
                granularity_str = "area"; break;
            case MeshGranularity::PerPrimaryFeatureObject:
                granularity_str = "primary"; break;
            case MeshGranularity::PerAtomicFeatureObject:
                granularity_str = "atomic"; break;
        };
        std::cout << "====== Testing converting to " << granularity_str << " ======" << std::endl;
        GranularityConvertOption option(granularity, 10);

        // ここで変換します。
        auto converted_model = GranularityConverter().convert(src_model_, option);

        const auto& expect = convert_expects_.at(granularity);
        checkModelBFS(converted_model, expect);
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfArea() {
    auto mesh = createTestMeshOfArea(test_indices_primary_and_atomic, test_city_obj_list_primary_and_atomic);

    auto model = Model();
    auto& gml_node = model.addNode(Node("gml_node"));
    auto& lod_node = gml_node.addChildNode(Node("lod_node"));
    auto& mesh_node = lod_node.addChildNode(Node("mesh_node"));
    mesh_node.setMesh(std::move(mesh));
    auto expects = createExpectsForTestMeshArea();
    auto ret = ModelConvertTestPatterns(std::move(model), expects);
    return ret;
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfPrimary() {
    auto area_patterns = createTestPatternsOfArea();
    auto option = GranularityConvertOption(MeshGranularity::PerPrimaryFeatureObject, 10);
    auto primary_model = GranularityConverter().convert(area_patterns.getModel(), option);
    auto primary_expect = area_patterns.getExpects();
    return {std::move(primary_model), primary_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfArea_OnlyRoot() {

    auto mesh = createTestMeshOfArea(test_indices_primary_and_atomic, test_city_obj_list_primary_and_atomic);

    // このメソッドに登場するsrc_modelは、すべて変換前のモデルです。
    // 変換後のモデルをデバッガで確認したい場合は、ModelForTest::testメソッド内にブレークポイントを貼ってください。
    auto src_model = Model();

    auto& root_node = src_model.addNode(Node("root_node"));
    root_node.setMesh(std::move(mesh));

    // テストの期待値からgmlノードとlodノードの分を除いて、ルートノードを追加します。
    auto expects = createExpectsForTestMeshArea();

    auto& expects_area = expects.at(MeshGranularity::PerCityModelArea);
    expects_area.at(0).expect_node_name_ = "combined";


    auto& expects_atomic = expects.at(MeshGranularity::PerAtomicFeatureObject);
    expects_atomic.eraseRange(0,2);

    auto& expects_primary = expects.at(MeshGranularity::PerPrimaryFeatureObject);
    expects_primary.eraseRange(0, 2);

    auto ret = ModelConvertTestPatterns(std::move(src_model), expects);

    return ret;
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfPrimary_OnlyRoot() {
    auto area_patterns = createTestPatternsOfArea_OnlyRoot();
    auto option = GranularityConvertOption(MeshGranularity::PerPrimaryFeatureObject, 10);
    auto primary_model = GranularityConverter().convert(area_patterns.getModel(), option);
    auto primary_expect = area_patterns.getExpects();
    return {std::move(primary_model), primary_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfAtomic_OnlyRoot() {
    auto area_patterns = createTestPatternsOfArea_OnlyRoot();
    auto option = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_patterns.getModel(), option);
    auto expects = area_patterns.getExpects();
    auto& expect_primary = expects.at(MeshGranularity::PerPrimaryFeatureObject);
    return {std::move(atomic_model), expects};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfPrimary_OnlyAtomicMesh() {
    auto area_patterns = createTestPatternsOfArea_OnlyAtomicMesh();
    auto option = GranularityConvertOption(MeshGranularity::PerPrimaryFeatureObject, 10);
    auto primary_model = GranularityConverter().convert(area_patterns.getModel(), option);
    auto primary_expect = area_patterns.getExpects();
    return {std::move(primary_model), primary_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfArea_OnlyAtomicMesh_Root() {
    auto mesh = createTestMeshOfArea(test_indices_only_atomic, test_city_obj_list_indices_only_atomic);
    auto model = Model();
    auto& node = model.addNode(Node("root_node"));
    node.setMesh(std::move(mesh));

    auto expects = createTestPatternsOfArea_OnlyAtomicMesh().getExpects();
    auto& expect_atomic = expects.at(MeshGranularity::PerAtomicFeatureObject);
    auto& expect_primary = expects.at(MeshGranularity::PerPrimaryFeatureObject);
    auto& expect_area = expects.at(MeshGranularity::PerCityModelArea);
    expect_atomic.eraseRange(0, 4);
    expect_atomic.at(0).expect_city_obj_list_ = {{{{0, -1}, "gml_id_not_found"}, {{0, 0}, "atomic-0-0"}}};
    expect_atomic.at(1).expect_city_obj_list_ = {{{{0, -1}, "gml_id_not_found"}, {{0, 0}, "atomic-0-1"}}};
    expect_atomic.at(2).expect_city_obj_list_ = {{{{0, -1}, "gml_id_not_found"}, {{0, 0}, "atomic-1-0"}}};
    expect_atomic.at(3).expect_city_obj_list_ = {{{{0, -1}, "gml_id_not_found"}, {{0, 0}, "atomic-1-1"}}};
    expect_primary.eraseRange(0, 2);
    expect_primary.at(0).expect_city_obj_list_ = {{{{0,-1}, "gml_id_not_found"},{{0,0}, "atomic-0-0"}, {{0,1},"atomic-0-1"}}};
    expect_area.at(0).expect_node_name_ = "combined";

    for(int i=0; i<expect_area.nodeCount(); i++) {
        int atomic_id = 0;
        CityObjectList expect_area_city_obj_list;
//        std::set<CityObjectIndex> expect_area_city_obj_id_set;

//        auto id_map = expect_area.at(i).expect_city_obj_list_.getIdMap();
//        for(auto& [_, gml_id] : id_map) {
//            auto city_obj_id = CityObjectIndex(0, atomic_id++);
//            expect_area_city_obj_list.add(city_obj_id, gml_id);
//            expect_area_city_obj_id_set.insert(city_obj_id);
//        }

        expect_area.at(i).expect_city_obj_list_ =
                {{{{0, -1}, "gml_id_not_found"},
                  {{0, 0}, "atomic-0-0"},
                  {{0, 1}, "atomic-0-1"},
                  {{0, 2}, "atomic-1-0"},
                  {{0, 3}, "atomic-1-1"}
                 }};
//        expect_area.at(i).expect_city_obj_list_ = expect_area_city_obj_list;
        expect_area.at(i).expect_city_obj_id_set_ = {{0,0}, {0,1}, {0,2}, {0,3}};
    }

    return {std::move(model), expects};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfPrimary_OnlyAtomicMesh_Root() {
    auto area_patterns = createTestPatternsOfArea_OnlyAtomicMesh_Root();
    auto atomic_expect = createTestPatternsOfAtomic_OnlyAtomicMesh_Root();
    auto options = GranularityConvertOption(MeshGranularity::PerPrimaryFeatureObject, 10);
    auto primary_model = GranularityConverter().convert(area_patterns.getModel(), options);
    return {std::move(primary_model), atomic_expect.getExpects()};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfAtomic_OnlyAtomicMesh_Root() {
    auto area_patterns = createTestPatternsOfArea_OnlyAtomicMesh_Root();
    auto options = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_patterns.getModel(), options);
    auto expects = area_patterns.getExpects();
    auto expect_primary = ModelExpect({
                                              NodeExpect("combined", true, 4 * 4,
                                                         {{0, 0},
                                                          {0, 1},
                                                          {0, 2},
                                                          {0, 3}},
                                                         {{
                                                                  {{0, -1}, "gml_id_not_found"},
                                                             {{0, 0}, "atomic-0-0"},
                                                             {{0, 1}, "atomic-0-1"},
                                                           {{0, 2}, "atomic-1-0"},
                                                           {{0, 3}, "atomic-1-1"}}},
                                                         {
                                                                 SubMesh(0, 5, "dummy_tex_path_0", nullptr),
                                                                 SubMesh(6, 11, "dummy_tex_path_1", nullptr),
                                                                 SubMesh(12, 17, "dummy_tex_path_2", nullptr),
                                                                 SubMesh(18, 23, "dummy_tex_path_3", nullptr)
                                                         })
                                      });
    expects.at(MeshGranularity::PerPrimaryFeatureObject) = expect_primary;
    return {std::move(atomic_model), expects};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfArea_WithoutCityObjList() {
    auto area_patterns = createTestPatternsOfArea();
    auto& model = area_patterns.getModel();

    // 幅優先探索で全MeshのCityObjectListを消します。
    auto queue = NodeQueue();
    for(int i=0; i<model.getRootNodeCount(); i++) {
        queue.push(NodePath({i}));
    }
    while(!queue.empty()) {
        auto node_path = queue.pop();
        auto* node = node_path.toNode(&model);
        auto* mesh = node->getMesh();
        if(mesh!=nullptr){
            mesh->setCityObjectList({});
        }
        for(int i=0; i<node->getChildCount(); i++){
            queue.push(node_path.plus(i));
        }
    }

    auto expect = createExpectsForTestMeshArea();

    auto& expect_atomic = expect.at(MeshGranularity::PerAtomicFeatureObject);
    expect_atomic.setExpectNodeNameRange("mesh_node", 2, 7);
    expect_atomic.setExpectGmlIdRange("gml_id_not_found", 2, 7);

    auto& expect_primary = expect.at(MeshGranularity::PerPrimaryFeatureObject);
    expect_primary.setExpectNodeNameRange("mesh_node", 2, 3);
    expect_primary.setExpectGmlIdRange("gml_id_not_found", 2, 3);

    auto& expect_area = expect.at(MeshGranularity::PerCityModelArea);
    expect_area.setExpectGmlIdRange("gml_id_not_found", 0, 0);

    return {std::move(model), expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfPrimary_WithoutCityObjList() {
    auto area_patterns = createTestPatternsOfArea_WithoutCityObjList();
    auto option = GranularityConvertOption(MeshGranularity::PerPrimaryFeatureObject, 10);
    auto primary_model = GranularityConverter().convert(area_patterns.getModel(), option);
    auto primary_expect = area_patterns.getExpects();
    return {std::move(primary_model), primary_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfAtomic_WithoutCityObjList() {
    auto area_patterns = createTestPatternsOfArea_OnlyAtomicMesh();
    auto option = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_patterns.getModel(), option);
    auto atomic_expect = area_patterns.getExpects();
    return {std::move(atomic_model), atomic_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfAtomic() {
    // テスト用モデルを手打ちコードで作るのは非常に手間なので、
    // 地域単位から最小地物単位への変換がうまくいっていることを前提に、変換したものをテストモデルとします。
    // もし前提機能が壊れた場合、他のテストで検出できるはずです。
    // その代わり、デバッガでブレークポイントを貼りながら実行する時は変換が2回行われ、Atomicから各種単位への変換は2回目のほうであることに留意してください。
    auto area_patterns = createTestPatternsOfArea();
    auto option = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_patterns.getModel(), option);
    // 期待する変換結果は地域単位と同じです。
    auto atomic_expect = area_patterns.getExpects();
    return {std::move(atomic_model), atomic_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfAtomic_OnlyAtomicMesh() {
    auto area_model = createTestPatternsOfArea_OnlyAtomicMesh();
    auto option = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_model.getModel(), option);
    // 期待する変換結果は地域単位と同じです。
    auto atomic_expect = area_model.getExpects();
    return {std::move(atomic_model), atomic_expect};
}

std::unique_ptr<Mesh> ModelConvertTestPatternsFactory::createTestMeshOfArea(std::vector<CityObjectIndex> city_object_indices, CityObjectList city_obj_list) {
    TVec3d base_pos = {0, 0, 0};
    unsigned int base_id = 0;
    std::vector<TVec3d> vertices;
    std::vector<unsigned int> indices;
    std::vector<TVec2f> uv1;
    std::vector<TVec2f> uv4;
    std::vector<SubMesh> sub_meshes;

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

    mesh->setCityObjectList(city_obj_list);
    return mesh;
}

ModelConvertTestPatterns::TGranularityToExpect ModelConvertTestPatternsFactory::createExpectsForTestMeshArea() {

    // メッシュを持たないオブジェクトはCityObjectIndexは無視されるので何の値でも良いです。
    const CityObjectIndex none_coi = {-999, -999};

    // 最小地物単位の場合
    auto atomic_expect = ModelExpect(
            std::vector<NodeExpect>{
                    NodeExpect("gml_node", false, 0, {{none_coi}}, {}, {}),
                    NodeExpect("lod_node", false, 0, {{none_coi}}, {}, {}),
                    NodeExpect("primary-0", true, 4,
                               {{{0, -1}}},
                               {{{{0, -1}, "primary-0"}}},
                               {SubMesh(0, 5, "dummy_tex_path_0", nullptr)}),
                    NodeExpect("primary-1", true, 4,
                               {{{0, -1}}},
                               {{{{0, -1}, "primary-1"}}},
                               {SubMesh(0, 5, "dummy_tex_path_5", nullptr)}),
                    NodeExpect("atomic-0-0", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "primary-0"}, {{0, 0}, "atomic-0-0"}}},
                               {SubMesh(0, 5, "dummy_tex_path_1", nullptr)}),
                    NodeExpect("atomic-0-1", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "primary-0"}, {{0, 0}, "atomic-0-1"}}},
                               {SubMesh(0, 5, "dummy_tex_path_2", nullptr)}),
                    NodeExpect("atomic-1-0", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "primary-1"}, {{0, 0}, "atomic-1-0"}}},
                               {SubMesh(0, 5, "dummy_tex_path_3", nullptr)}),
                    NodeExpect("atomic-1-1", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "primary-1"}, {{0, 0}, "atomic-1-1"}}},
                               {SubMesh(0, 5, "dummy_tex_path_4", nullptr)})
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

    auto convert_expects = ModelConvertTestPatterns::TGranularityToExpect{
            {MeshGranularity::PerAtomicFeatureObject, atomic_expect},
            {MeshGranularity::PerPrimaryFeatureObject, primary_expect},
            {MeshGranularity::PerCityModelArea, area_expect}
    };
    return convert_expects;
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfArea_OnlyAtomicMesh() {
    auto mesh = createTestMeshOfArea(test_indices_only_atomic, test_city_obj_list_indices_only_atomic);
    auto model = Model();
    auto& gml_node = model.addNode(Node("gml_node"));
    auto& lod_node = gml_node.addChildNode(Node("lod_node"));
    auto& mesh_node = lod_node.addChildNode(Node("mesh_node"));
    mesh_node.setMesh(std::move(mesh));

    const CityObjectIndex none_coi = {-999, -999};

    // 最小地物単位の場合
    auto atomic_expect = ModelExpect(
            std::vector<NodeExpect>{
                    NodeExpect("gml_node", false, 0, {{none_coi}}, {}, {}),
                    NodeExpect("lod_node", false, 0, {{none_coi}}, {}, {}),
                    NodeExpect(/*"primary-0"*/"mesh_node", false, 0, {{none_coi}}, {}, {}),
                    NodeExpect(/*"primary-1"*/"mesh_node", false, 0, {{none_coi}}, {}, {}),
                    NodeExpect("atomic-0-0", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "mesh_node"}, {{0, 0}, "atomic-0-0"}}},
                               {SubMesh(0, 5, "dummy_tex_path_0", nullptr)}),
                    NodeExpect("atomic-0-1", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "mesh_node"}, {{0, 0}, "atomic-0-1"}}},
                               {SubMesh(0, 5, "dummy_tex_path_1", nullptr)}),
                    NodeExpect("atomic-1-0", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "mesh_node"}, {{0, 0}, "atomic-1-0"}}},
                               {SubMesh(0, 5, "dummy_tex_path_2", nullptr)}),
                    NodeExpect("atomic-1-1", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "mesh_node"}, {{0, 0}, "atomic-1-1"}}},
                               {SubMesh(0, 5, "dummy_tex_path_3", nullptr)})
            });

    // 主要地物の場合
    auto primary_expect = ModelExpect(
            std::vector<NodeExpect>{
                    NodeExpect("gml_node", false, 0, {{}}, {}, {}),
                    NodeExpect("lod_node", false, 0, {{}}, {}, {}),
                    NodeExpect(/*"primary-0"*/"mesh_node", true, 4*2, {{{0,0}, {0,1}}},
                                              {{{{0,-1}, "mesh_node"},{{0,0}, "atomic-0-0"}, {{0,1},"atomic-0-1"}}},
                               {
                                SubMesh(0,5,"dummy_tex_path_0", nullptr),
                                SubMesh(6,11,"dummy_tex_path_1",nullptr)
                               }),
                    NodeExpect(/*"primary-1"*/"mesh_node", true, 4*2, {{{0,0}, {0,1}}},
                                              {{{ {0,-1}, "mesh_node"},{{0,0}, "atomic-1-0"}, {{0,1},"atomic-1-1"}}},
                               {
                                SubMesh(0,5,"dummy_tex_path_2", nullptr),
                                SubMesh(6,11,"dummy_tex_path_3",nullptr)}
                    )
            });

    // 地域単位の場合
    auto area_expect = ModelExpect(
            std::vector<NodeExpect>{
                    NodeExpect("gml_node", true, 4 * 4,
                               {{
                                        {0, 0},
                                        {0, 1},
                                        {1, 0},
                                        {1, 1}
                                }},
                               {{
                                        {{0, -1}, "mesh_node"},
                                        {{1, -1}, "mesh_node"},
                                        {{0, 0}, "atomic-0-0"},
                                        {{0, 1}, "atomic-0-1"},
                                        {{1, 0}, "atomic-1-0"},
                                        {{1, 1}, "atomic-1-1"}
                                }},
                               {
                                       SubMesh(0, 5, "dummy_tex_path_0", nullptr),
                                       SubMesh(6, 11, "dummy_tex_path_1", nullptr),
                                       SubMesh(12, 17, "dummy_tex_path_2", nullptr),
                                       SubMesh(18, 23, "dummy_tex_path_3", nullptr)
                               }
                    )}
    );

    auto convert_expects = ModelConvertTestPatterns::TGranularityToExpect{
            {MeshGranularity::PerAtomicFeatureObject, atomic_expect},
            {MeshGranularity::PerPrimaryFeatureObject, primary_expect},
            {MeshGranularity::PerCityModelArea, area_expect}
    };
    return {std::move(model), convert_expects};
}
