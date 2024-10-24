#include "gtest/gtest.h"
#include "model_convert_test_patterns.h"
#include <plateau/polygon_mesh/model.h>
#include <plateau/granularity_convert/granularity_converter.h>
#include <plateau/granularity_convert/node_queue.h>
#include <cassert>

using namespace plateau::polygonMesh;
using namespace plateau::granularityConvert;

namespace {
    /// MeshのCityObjectListについて、AtomicとPrimaryの両方のIDがある場合、Primaryのみを除きます。
    /// こうすることで、ゲームエンジンから送られる情報とテストデータを合わせます。
    void removePrimaryFromCityObjectListWithAtomic(Mesh& mesh) {
        auto& list = mesh.getCityObjectList();
        auto primary_size = list.getAllPrimaryIndices().size();
        auto atomic_size = list.size() - primary_size;
        if(primary_size == 0 || atomic_size ==0) return;

        CityObjectList next_list;
        for(const auto& [index, gml_id] : list) {
            if(index.atomic_index == CityObjectIndex::invalidIndex()) continue;
            next_list.add(index, gml_id);
        }
        mesh.setCityObjectList(next_list);
    }
}

void NodeExpect::checkNode(const Node* node) const {
    EXPECT_EQ(node->getName(), expect_node_name_);
    EXPECT_EQ(!(node->getMesh() == nullptr), expect_have_mesh_);
    std::cout << node->getName() << std::endl;
    if(node->getMesh() != nullptr) {
        const auto mesh = node->getMesh();
        const auto vertex_count = mesh->getVertices().size();
        EXPECT_EQ(vertex_count, expect_vertex_count_);
        std::set<CityObjectIndex> obj_indices_in_mesh;
        for(int i=0; i<vertex_count; i++){
            const auto city_obj_id = CityObjectIndex::fromUV(mesh->getUV4().at(i));
            obj_indices_in_mesh.insert(city_obj_id);
        }
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

void ModelConvertTestPatterns::test(ConvertGranularity granularity) {
    std::cout << "====== Model Test Start ======"<< std::endl;
    std::cout << "rootNodeCount: " << src_model_.getRootNodeCount() << std::endl;
        std::string granularity_str;
        switch(granularity) {
            case ConvertGranularity::PerCityModelArea:
                granularity_str = "area"; break;
            case ConvertGranularity::PerPrimaryFeatureObject:
                granularity_str = "primary"; break;
            case ConvertGranularity::PerAtomicFeatureObject:
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
    auto expects = createExpectsForTestMeshArea("primary-0_combined");
    auto ret = ModelConvertTestPatterns(std::move(model), expects);
    return ret;
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfPrimary() {
    auto area_patterns = createTestPatternsOfArea();
    auto option = GranularityConvertOption(ConvertGranularity::PerPrimaryFeatureObject, 10);
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
    auto expects = createExpectsForTestMeshArea("primary-0_combined");

    auto& expects_to_area = expects.at(ConvertGranularity::PerCityModelArea);
    expects_to_area.eraseRange(0, 2);
    expects_to_area.at(0).expect_node_name_ = "primary-0_combined";
    expects_to_area.at(0).expect_city_obj_list_ =
            {{{{0, -1}, "primary-0"},
              {{0, 0}, "atomic-0-0"},
              {{0, 1}, "atomic-0-1"},
              {{1, -1}, "primary-1"},
              {{1, 0}, "atomic-1-0"},
              {{1, 1}, "atomic-1-1"}}};
    expects_to_area.at(0).expect_sub_meshes_ =
            {
                    SubMesh(0, 5, "dummy_tex_path_0", nullptr),
                    SubMesh(6, 11, "dummy_tex_path_1", nullptr),
                    SubMesh(12, 17, "dummy_tex_path_2", nullptr),
                    SubMesh(18, 23, "dummy_tex_path_5", nullptr),
                    SubMesh(24, 29, "dummy_tex_path_3", nullptr),
                    SubMesh(30, 35, "dummy_tex_path_4", nullptr)
            };


    auto& expects_to_atomic = expects.at(ConvertGranularity::PerAtomicFeatureObject);
    expects_to_atomic.eraseRange(0, 2);

    auto& expects_to_primary = expects.at(ConvertGranularity::PerPrimaryFeatureObject);
    expects_to_primary.eraseRange(0, 2);

    auto ret = ModelConvertTestPatterns(std::move(src_model), expects);

    return ret;
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfPrimary_OnlyRoot() {
    auto area_patterns = createTestPatternsOfArea_OnlyRoot();
    auto option = GranularityConvertOption(ConvertGranularity::PerPrimaryFeatureObject, 10);
    auto primary_model = GranularityConverter().convert(area_patterns.getModel(), option);
    auto primary_expect = area_patterns.getExpects();
    return {std::move(primary_model), primary_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfAtomic_OnlyRoot() {
    auto area_patterns = createTestPatternsOfArea_OnlyRoot();
    auto option = GranularityConvertOption(ConvertGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_patterns.getModel(), option);
    adjustForTestModelForAtomic(atomic_model);
    auto expects = area_patterns.getExpects();
    auto& expect_primary = expects.at(ConvertGranularity::PerPrimaryFeatureObject);

    return {std::move(atomic_model), expects};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfArea_MultipleGMLs() {
    auto model = Model();
    auto& gml1 = model.addNode(Node("gml1"));
    auto& lod1 = gml1.addChildNode(Node("lod1"));
    auto mesh1 = createTestMeshOfArea(test_indices_primary_and_atomic, test_city_obj_list_primary_and_atomic);
    auto& mesh1_node = lod1.addChildNode(Node("mesh1"));
    auto& gml2 = model.addNode(Node("gml2"));
    auto& lod2 = gml2.addChildNode(Node("lod2"));
    auto& mesh2_node = lod2.addChildNode(Node("mesh2"));
    auto mesh2 = createTestMeshOfArea(test_indices_primary_and_atomic, test_city_obj_list_primary_and_atomic);
    mesh1_node.setMesh(std::move(mesh1));
    mesh2_node.setMesh(std::move(mesh2));

    auto atomic_expect = ModelExpect(std::vector<NodeExpect>{
        NodeExpect("gml1", false, 0, {}, {}, {}),
        NodeExpect("gml2", false, 0, {}, {}, {}),
        NodeExpect("lod1", false, 0, {}, {}, {}),
        NodeExpect("lod2", false, 0, {}, {}, {}),
        NodeExpect("primary-0", true, 4,
                   {{{0, -1}}},
                   {{{{0, -1}, "primary-0"}}},
                   {SubMesh(0, 5, "dummy_tex_path_0", nullptr)}),
        NodeExpect("primary-1", true, 4,
                   {{{0, -1}}},
                   {{{{0, -1}, "primary-1"}}},
                   {SubMesh(0, 5, "dummy_tex_path_5", nullptr)}),
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
                   {SubMesh(0, 5, "dummy_tex_path_4", nullptr)}),
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

    auto primary_expect = ModelExpect(std::vector<NodeExpect>{
            NodeExpect("gml1", false, 0, {}, {}, {}),
            NodeExpect("gml2", false, 0, {}, {}, {}),
            NodeExpect("lod1", false, 0, {}, {}, {}),
            NodeExpect("lod2", false, 0, {}, {}, {}),
            NodeExpect("primary-0", true, 4 * 3, {{{0, -1}, {0, 0}, {0, 1}}},
                       {{{{0, -1}, "primary-0"}, {{0, 0}, "atomic-0-0"}, {{0, 1}, "atomic-0-1"}}},
                       {SubMesh(0, 5, "dummy_tex_path_0", nullptr),
                        SubMesh(6, 11, "dummy_tex_path_1", nullptr),
                        SubMesh(12, 17, "dummy_tex_path_2", nullptr)
                       }),
            NodeExpect("primary-1", true, 4 * 3, {{{0, -1}, {0, 0}, {0, 1}}},
                       {{{{0, -1}, "primary-1"}, {{0, 0}, "atomic-1-0"}, {{0, 1}, "atomic-1-1"}}},
                       {SubMesh(0, 5, "dummy_tex_path_5", nullptr),
                        SubMesh(6, 11, "dummy_tex_path_3", nullptr),
                        SubMesh(12, 17, "dummy_tex_path_4", nullptr)}),
            // 2つ目
            NodeExpect("primary-0", true, 4 * 3, {{{0, -1}, {0, 0}, {0, 1}}},
                       {{{{0, -1}, "primary-0"}, {{0, 0}, "atomic-0-0"}, {{0, 1}, "atomic-0-1"}}},
                       {SubMesh(0, 5, "dummy_tex_path_0", nullptr),
                        SubMesh(6, 11, "dummy_tex_path_1", nullptr),
                        SubMesh(12, 17, "dummy_tex_path_2", nullptr)
                       }),
            NodeExpect("primary-1", true, 4 * 3, {{{0, -1}, {0, 0}, {0, 1}}},
                       {{{{0, -1}, "primary-1"}, {{0, 0}, "atomic-1-0"}, {{0, 1}, "atomic-1-1"}}},
                       {SubMesh(0, 5, "dummy_tex_path_5", nullptr),
                        SubMesh(6, 11, "dummy_tex_path_3", nullptr),
                        SubMesh(12, 17, "dummy_tex_path_4", nullptr)})
    });

    auto area_expect = ModelExpect(std::vector<NodeExpect>{
            NodeExpect("gml1", false, 0, {}, {}, {}),
            NodeExpect("gml2", false, 0, {}, {}, {}),
            NodeExpect("lod1", false, 0, {}, {}, {}),
            NodeExpect("lod2", false, 0, {}, {}, {}),
            NodeExpect("primary-0_combined", true, 4 * 6,
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
            ),
            // 2つ目
            NodeExpect("primary-0_combined", true, 4 * 6,
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
                       })

    });
    ModelConvertTestPatterns::TGranularityToExpect expects = {
            {ConvertGranularity::PerAtomicFeatureObject, atomic_expect},
            {ConvertGranularity::PerPrimaryFeatureObject, primary_expect},
            {ConvertGranularity::PerCityModelArea, area_expect}
    };
    return {std::move(model), expects};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfPrimary_OnlyAtomicMesh() {
    auto area_patterns = createTestPatternsFromArea_OnlyAtomicMesh();
    auto option = GranularityConvertOption(ConvertGranularity::PerPrimaryFeatureObject, 10);
    auto primary_model = GranularityConverter().convert(area_patterns.getModel(), option);
    auto primary_expect = area_patterns.getExpects();
    return {std::move(primary_model), primary_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfArea_OnlyAtomicMesh_Root() {
    auto mesh = createTestMeshOfArea(test_indices_only_atomic, test_city_obj_list_indices_only_atomic);
    auto model = Model();
    auto& node = model.addNode(Node("root_node"));
    node.setMesh(std::move(mesh));

    auto expects = createTestPatternsFromArea_OnlyAtomicMesh().getExpects();

    auto& expect_to_atomic = expects.at(ConvertGranularity::PerAtomicFeatureObject);
    expect_to_atomic.eraseRange(0, 2);

    auto& expect_to_primary = expects.at(ConvertGranularity::PerPrimaryFeatureObject);
    expect_to_primary.eraseRange(0, 2);
    expect_to_primary.at(0).expect_city_obj_list_ = {
            {{{0, -1}, "primary-0"}, {{0, 0}, "atomic-0-0"}, {{0, 1}, "atomic-0-1"}}};

    auto& expect_to_area = expects.at(ConvertGranularity::PerCityModelArea);
    expect_to_area.eraseRange(0, 2);
    expect_to_area.at(0).expect_node_name_ = "primary-0_combined";
//    CityObjectList expect_area_city_obj_list;
    expect_to_area.at(0).expect_city_obj_id_set_ = {{0, 0},
                                                    {0, 1},
                                                    {1, 0},
                                                    {1, 1}};
    expect_to_area.at(0).expect_city_obj_list_ =
            {{{{0, -1}, "primary-0"},
              {{0, 0}, "atomic-0-0"},
              {{0, 1}, "atomic-0-1"},
              {{1, -1}, "primary-1"},
              {{1, 0}, "atomic-1-0"},
              {{1, 1}, "atomic-1-1"}}};
    expect_to_area.at(0).expect_sub_meshes_ =
            {
                    SubMesh(0, 5, "dummy_tex_path_0", nullptr),
                    SubMesh(6, 11, "dummy_tex_path_1", nullptr),
                    SubMesh(12, 17, "dummy_tex_path_2", nullptr),
                    SubMesh(18, 23, "dummy_tex_path_3", nullptr)
            };

    return {std::move(model), expects};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfPrimary_OnlyAtomicMesh_Root() {
    auto area_patterns = createTestPatternsOfArea_OnlyAtomicMesh_Root();
    auto atomic_expect = createTestPatternsOfAtomic_OnlyAtomicMesh_Root();
    auto options = GranularityConvertOption(ConvertGranularity::PerPrimaryFeatureObject, 10);
    auto primary_model = GranularityConverter().convert(area_patterns.getModel(), options);
    return {std::move(primary_model), atomic_expect.getExpects()};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfAtomic_OnlyAtomicMesh_Root() {
    auto area_patterns = createTestPatternsOfArea_OnlyAtomicMesh_Root();
    auto options = GranularityConvertOption(ConvertGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_patterns.getModel(), options);
    adjustForTestModelForAtomic(atomic_model);
    auto expects = area_patterns.getExpects();

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

    auto expect = createExpectsForTestMeshArea("mesh_node_combined");

    auto& expect_to_atomic = expect.at(ConvertGranularity::PerAtomicFeatureObject);
    expect_to_atomic.setExpectNodeNameRange("mesh_node", 2, 7);
    expect_to_atomic.setExpectGmlIdRange("gml_id_not_found", 2, 7);
    expect_to_atomic.at(4).expect_city_obj_list_.getIdMap().at(CityObjectIndex(0,-1)) = "mesh_node";
    expect_to_atomic.at(5).expect_city_obj_list_.getIdMap().at(CityObjectIndex(0,-1)) = "mesh_node";

    auto& expect_to_primary = expect.at(ConvertGranularity::PerPrimaryFeatureObject);
    expect_to_primary.setExpectNodeNameRange("mesh_node", 2, 3);
    expect_to_primary.setExpectGmlIdRange("gml_id_not_found", 2, 3);
    expect_to_primary.at(2).expect_node_name_ = "gml_id_not_found_0";
    expect_to_primary.at(3).expect_node_name_ = "gml_id_not_found_1";
    expect_to_primary.at(2).expect_city_obj_list_.getIdMap().at(CityObjectIndex(0, -1)) = "mesh_node";

    auto& expect_to_area = expect.at(ConvertGranularity::PerCityModelArea);
    expect_to_area.at(2).expect_city_obj_list_ =
            {{{{0, -1}, "mesh_node"},
              {{0, 0}, "gml_id_not_found"},
              {{0, 1}, "gml_id_not_found"},
              {{1, -1}, "gml_id_not_found"}, // FIXME
              {{1, 0}, "gml_id_not_found"},
              {{1, 1}, "gml_id_not_found"}}};
//    expect_area.setExpectGmlIdRange("gml_id_not_found", 0, 0);


    return {std::move(model), expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfPrimary_WithoutCityObjList() {
    auto area_patterns = createTestPatternsOfArea_WithoutCityObjList();
    auto option = GranularityConvertOption(ConvertGranularity::PerPrimaryFeatureObject, 10);
    auto primary_model = GranularityConverter().convert(area_patterns.getModel(), option);
    auto primary_expect = area_patterns.getExpects();

    auto& primary_to_primary = primary_expect.at(ConvertGranularity::PerPrimaryFeatureObject);
    primary_to_primary.at(2).expect_node_name_ = "mesh_node";
//    primary_to_primary.at(2).expect_node_name_ = "gml_id_not_found_0";
    primary_to_primary.at(2).expect_city_obj_list_ =
            {{{{0, -1}, "mesh_node"},
              {{0, 0}, "gml_id_not_found"},
              {{0, 1}, "gml_id_not_found"}}};
    primary_to_primary.at(3).expect_node_name_ = "gml_id_not_found_0";

    auto& primary_to_atomic = primary_expect.at(ConvertGranularity::PerAtomicFeatureObject);
//    primary_to_atomic.at(0).expect_node_name_ = "gml_id_not_found_0";
//    primary_to_atomic.at(1).expect_node_name_ = "gml_id_not_found_1";
    primary_to_atomic.at(2).expect_node_name_ = "mesh_node";
    primary_to_atomic.at(2).expect_city_obj_list_ =
            {{{{0, -1}, "mesh_node"}}};
    primary_to_atomic.at(3).expect_node_name_ = "gml_id_not_found_1";
    primary_to_atomic.at(4).expect_city_obj_list_ =
            {{{{0, -1}, "mesh_node"}, {{0, 0}, "gml_id_not_found"}}};
    primary_to_atomic.at(4).expect_node_name_ = "gml_id_not_found_0";
    primary_to_atomic.at(5).expect_city_obj_list_ =
            {{{{0, -1}, "mesh_node"}, {{0, 0}, "gml_id_not_found"}}};
    primary_to_atomic.at(5).expect_node_name_ = "gml_id_not_found_0";
    primary_to_atomic.at(6).expect_node_name_ = "gml_id_not_found_1";
    primary_to_atomic.at(7).expect_node_name_ = "gml_id_not_found_1";


    return {std::move(primary_model), primary_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfAtomic_WithoutCityObjList() {
    auto area_patterns = createTestPatternsFromArea_OnlyAtomicMesh();
    auto option = GranularityConvertOption(ConvertGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_patterns.getModel(), option);
    adjustForTestModelForAtomic(atomic_model);
    auto atomic_expect = area_patterns.getExpects();
    return {std::move(atomic_model), atomic_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfAtomic() {
    // テスト用モデルを手打ちコードで作るのは非常に手間なので、
    // 地域単位から最小地物単位への変換がうまくいっていることを前提に、変換したものをテストモデルとします。
    // もし前提機能が壊れた場合、他のテストで検出できるはずです。
    // その代わり、デバッガでブレークポイントを貼りながら実行する時は変換が2回行われ、Atomicから各種単位への変換は2回目のほうであることに留意してください。
    auto area_patterns = createTestPatternsOfArea();
    auto option = GranularityConvertOption(ConvertGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_patterns.getModel(), option);
    adjustForTestModelForAtomic(atomic_model);
    // 期待する変換結果は地域単位と同じです。
    auto atomic_expect = area_patterns.getExpects();
    return {std::move(atomic_model), atomic_expect};
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsOfAtomic_OnlyAtomicMesh() {
    auto area_model = createTestPatternsFromArea_OnlyAtomicMesh();
    auto option = GranularityConvertOption(ConvertGranularity::PerAtomicFeatureObject, 10);
    auto atomic_model = GranularityConverter().convert(area_model.getModel(), option);
    adjustForTestModelForAtomic(atomic_model);
    // 期待する変換結果は地域単位と同じです。
    auto atomic_expect = area_model.getExpects();
    return {std::move(atomic_model), atomic_expect};
}

std::unique_ptr<Mesh> ModelConvertTestPatternsFactory::createTestMeshOfArea(
        std::vector<CityObjectIndex> city_object_indices, CityObjectList city_obj_list) {
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

void ModelConvertTestPatternsFactory::adjustForTestModelForAtomic(plateau::polygonMesh::Model& model) {
    NodeQueue queue;
    queue.pushRoot(&model);
    while(!queue.empty()) {
        auto node_path = queue.pop();
        auto node = node_path.toNode(&model);
        node->setGranularityConvertInfo(false, true);
        auto mesh = node->getMesh();
        if(mesh != nullptr) {
            removePrimaryFromCityObjectListWithAtomic(*mesh);
        }
        queue.pushChildren(node_path, &model);
    }
}

ModelConvertTestPatterns::TGranularityToExpect ModelConvertTestPatternsFactory::createExpectsForTestMeshArea(const std::string& expect_mesh_node_name_area) {

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
                    NodeExpect("gml_node", false, 0, {}, {}, {}),
                    NodeExpect("lod_node", false, 0, {}, {}, {}),
                    NodeExpect(expect_mesh_node_name_area, true, 4 * 6,
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
            {ConvertGranularity::PerAtomicFeatureObject, atomic_expect},
            {ConvertGranularity::PerPrimaryFeatureObject, primary_expect},
            {ConvertGranularity::PerCityModelArea, area_expect}
    };
    return convert_expects;
}

ModelConvertTestPatterns ModelConvertTestPatternsFactory::createTestPatternsFromArea_OnlyAtomicMesh() {
    auto mesh = createTestMeshOfArea(test_indices_only_atomic, test_city_obj_list_indices_only_atomic);
    auto model = Model();
    auto& gml_node = model.addNode(Node("gml_node"));
    auto& lod_node = gml_node.addChildNode(Node("lod_node"));
    auto& mesh_node = lod_node.addChildNode(Node("mesh_node"));
    mesh_node.setMesh(std::move(mesh));

    // 最小地物単位の場合
    auto atomic_expect = ModelExpect(
            std::vector<NodeExpect>{
                    NodeExpect("gml_node", false, 0, {{}}, {}, {}),
                    NodeExpect("lod_node", false, 0, {{}}, {}, {}),
                    NodeExpect("primary-0", true, 0, {},
                               {{{{0, -1}, "primary-0"}}},
                               {}),
                    NodeExpect("primary-1", true, 0, {},
                               {{{{0, -1}, "primary-1"}}},
                               {}),
                    NodeExpect("atomic-0-0", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "primary-0"}, {{0, 0}, "atomic-0-0"}}},
                               {SubMesh(0, 5, "dummy_tex_path_0", nullptr)}),
                    NodeExpect("atomic-0-1", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "primary-0"}, {{0, 0}, "atomic-0-1"}}},
                               {SubMesh(0, 5, "dummy_tex_path_1", nullptr)}),
                    NodeExpect("atomic-1-0", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "primary-1"}, {{0, 0}, "atomic-1-0"}}},
                               {SubMesh(0, 5, "dummy_tex_path_2", nullptr)}),
                    NodeExpect("atomic-1-1", true, 4,
                               {{{0, 0}}},
                               {{{{0, -1}, "primary-1"}, {{0, 0}, "atomic-1-1"}}},
                               {SubMesh(0, 5, "dummy_tex_path_3", nullptr)})
            });

    // 主要地物の場合
    auto primary_expect = ModelExpect(
            std::vector<NodeExpect>{
                    NodeExpect("gml_node", false, 0, {{}}, {}, {}),
                    NodeExpect("lod_node", false, 0, {{}}, {}, {}),
                    NodeExpect("primary-0", true, 4*2, {{{0,0}, {0,1}}},
                                              {{{{0,-1}, "primary-0"},{{0,0}, "atomic-0-0"}, {{0,1},"atomic-0-1"}}},
                               {
                                SubMesh(0,5,"dummy_tex_path_0", nullptr),
                                SubMesh(6,11,"dummy_tex_path_1",nullptr)
                               }),
                    NodeExpect("primary-1", true, 4*2, {{{0,0}, {0,1}}},
                                              {{{ {0,-1}, "primary-1"},{{0,0}, "atomic-1-0"}, {{0,1},"atomic-1-1"}}},
                               {
                                SubMesh(0,5,"dummy_tex_path_2", nullptr),
                                SubMesh(6,11,"dummy_tex_path_3",nullptr)}
                    )
            });

    // 地域単位の場合
    auto area_expect = ModelExpect(
            std::vector<NodeExpect>{
                    NodeExpect("gml_node", false, 0, {}, {}, {}),
                    NodeExpect("lod_node", false, 0, {}, {}, {}),
                    NodeExpect("primary-0_combined", true, 4 * 4,
                               {{
                                        {0, 0},
                                        {0, 1},
                                        {1, 0},
                                        {1, 1}
                                }},
                               {{
                                        {{0, -1}, "primary-0"},
                                        {{1, -1}, "primary-1"},
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
            {ConvertGranularity::PerAtomicFeatureObject, atomic_expect},
            {ConvertGranularity::PerPrimaryFeatureObject, primary_expect},
            {ConvertGranularity::PerCityModelArea, area_expect}
    };
    return {std::move(model), convert_expects};
}
