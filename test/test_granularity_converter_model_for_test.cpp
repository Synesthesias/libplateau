#include "gtest/gtest.h"
#include "test_granularity_converter_model_for_test.h"
#include <plateau/polygon_mesh/model.h>
#include <plateau/granularity_convert/granularity_converter.h>
#include <queue>
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

    auto expect_size = expect.nodeCount();

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

void ModelForTest::test(MeshGranularity granularity) {
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

        // ここで変換します
        auto converted_model = GranularityConverter().convert(src_model_, option);

        const auto& expect = convert_expects_.at(granularity);
        checkModelBFS(converted_model, expect);
}
