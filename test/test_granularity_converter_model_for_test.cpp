#include "gtest/gtest.h"
#include "test_granularity_converter_model_for_test.h"
#include <plateau/polygon_mesh/model.h>
#include <plateau/granularity_convert/granularity_converter.h>
#include <queue>
#include <cassert>

using namespace plateau::polygonMesh;
using namespace plateau::granularityConvert;

size_t ModelExpect::nodeSize() const {
    auto expect_size = expect_nodes_name_.size();
    assert(expect_size == expect_have_mesh_.size());
    assert(expect_size == expect_have_mesh_.size());
    assert(expect_size ==  expect_vertices_count_.size());
    assert(expect_size ==  expect_city_objs_id_set_.size());
    assert(expect_size == expect_city_objs_list_.size());
    assert(expect_size == expect_sub_meshes_list_.size());
    return expect_size;
}

/// 引数で与えられたModelの各Nodeを幅優先探索の順番で調べ、引数の各vector.at(index)と一致するかどうか調べます。
void checkModelBFS(const Model& model, const ModelExpect& expect) {

    auto expect_size = expect.nodeSize();

    std::queue<const plateau::polygonMesh::Node*> queue;

    for(int i=0; i<model.getRootNodeCount(); i++){
        queue.push(&model.getRootNodeAt(i));
    }

    int node_index = 0;
    for(; !queue.empty(); node_index++){
        const auto node = queue.front();
        queue.pop();

        EXPECT_EQ(node->getName(), expect.expect_nodes_name_.at(node_index));
        EXPECT_EQ(!(node->getMesh() == nullptr), expect.expect_have_mesh_.at(node_index));
        if(node->getMesh() != nullptr) {
            const auto mesh = node->getMesh();
            const auto vertex_count = mesh->getVertices().size();
            EXPECT_EQ(vertex_count, expect.expect_vertices_count_.at(node_index));
            std::set<CityObjectIndex> obj_indices_in_mesh;
            for(int i=0; i<vertex_count; i++){
                const auto city_obj_id = CityObjectIndex::fromUV(mesh->getUV4().at(i));
                obj_indices_in_mesh.insert(city_obj_id);
            }
            EXPECT_EQ(obj_indices_in_mesh, expect.expect_city_objs_id_set_.at(node_index));
            EXPECT_EQ(mesh->getCityObjectList(), expect.expect_city_objs_list_.at(node_index));

            // SubMeshの一致を確認します。
            const auto& sub_meshes = mesh->getSubMeshes();
            const auto& expect_sub_meshes = expect.expect_sub_meshes_list_.at(node_index);
            EXPECT_EQ(sub_meshes.size(), expect_sub_meshes.size());
            for(int i=0; i<sub_meshes.size(); i++) {
                EXPECT_EQ(sub_meshes.at(i), expect_sub_meshes.at(i));
            }

            EXPECT_EQ(mesh->getUV4().size(), mesh->getVertices().size()) << "UV4 Exists";
        }

        for(int i=0; i<node->getChildCount(); i++){
            queue.push(&node->getChildAt(i));
        }

    }
    EXPECT_EQ(node_index, expect.expect_nodes_name_.size());
}

void ModelForTest::test() {
    std::cout << "====== Model Test Start ======"<< std::endl;
    std::cout << "rootNodeCount: " << src_model_.getRootNodeCount() << std::endl;
    for(const auto& [granularity, expect] : convert_expects_) {
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
        auto converted_model = GranularityConverter().convert(src_model_, option);
        checkModelBFS(converted_model, expect);
    }
}
