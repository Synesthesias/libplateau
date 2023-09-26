#include "gtest/gtest.h"
#include "plateau/polygon_mesh/model.h"
#include <plateau/granularity_convert/granularity_converter.h>
#include <queue>

using namespace plateau::granularityConvert;
using namespace plateau::polygonMesh;

class GranularityConverterTest : public ::testing::Test {
public:
};

namespace {

    Model createTestModelOfAreaGranularity() {
        TVec3d base_pos = {0, 0, 0};
        unsigned int base_id = 0;
        std::vector<TVec3d> vertices;
        std::vector<unsigned int> indices;
        std::vector<TVec2f> uv1;
        std::vector<TVec2f> uv4;

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

            indices.insert(indices.end(), {
                    base_id, base_id + 1, base_id + 2,
                    base_id, base_id + 2, base_id + 3
            });

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

        auto city_objs = CityObjectList();
        city_objs.add({0,-1}, "primary-0");
        city_objs.add({0, 0}, "atomic-0-0");
        city_objs.add({0, 1}, "atomic-0-1");
        city_objs.add({1, -1}, "primary-1");
        city_objs.add({1, 0}, "atomic-1-0");
        city_objs.add({1, 1}, "atomic-1-1");
        mesh->setCityObjectList(city_objs);

        auto node = Node("node");
        node.setMesh(std::move(mesh));
        auto model = Model();
        model.addNode(std::move(node));

        return model;
    }
}

namespace {

    /// 引数で与えられたModelの各Nodeを幅優先探索の順番で調べ、引数の各vector.at(index)と一致するかどうか調べます。
    void checkModelBFS(const Model& model,
                       const std::vector<std::string>& expect_node_name,
                       const std::vector<bool>& expect_has_mesh,
                       const std::vector<int>& expect_vertex_count, // expect_vertex_countとexpect_city_obj_idはMeshを持たないノードに関しては無視されます。
                       const std::vector<CityObjectIndex>& expect_city_obj_id,
                       const std::vector<CityObjectList>& expect_city_obj_list){
        std::queue<const Node*> queue;
        for(int i=0; i<model.getRootNodeCount(); i++){
            queue.push(&model.getRootNodeAt(i));
        }

        for(int node_index = 0; !queue.empty(); node_index++){
            const auto node = queue.front();
            queue.pop();

            EXPECT_EQ(node->getName(), expect_node_name.at(node_index));
            EXPECT_EQ(!(node->getMesh() == nullptr), expect_has_mesh.at(node_index));
            if(node->getMesh() != nullptr) {
                const auto mesh = node->getMesh();
                const auto vertex_count = mesh->getVertices().size();
                EXPECT_EQ(vertex_count, expect_vertex_count.at(node_index));
                for(int i=0; i<vertex_count; i++){
                    const auto city_obj_id = CityObjectIndex::fromUV(mesh->getUV4().at(i));
                    EXPECT_EQ(city_obj_id, expect_city_obj_id.at(node_index));
                }
                EXPECT_EQ(mesh->getCityObjectList(), expect_city_obj_list.at(node_index));
            }

            for(int i=0; i<node->getChildCount(); i++){
                queue.push(&node->getChildAt(i));
            }
        }
    }
}

TEST_F(GranularityConverterTest, convertFromAreaToAtomic) { // NOLINT
    const auto area_model = createTestModelOfAreaGranularity();
    const auto option = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
    auto converter = GranularityConverter();
    auto atomic_model = converter.convert(area_model, option);

    const auto expect_city_obj_list = CityObjectList();

    checkModelBFS(atomic_model,
                  {
                        "node",
                        "primary-0",
                        "primary-1",
                        "atomic-0-0",
                        "atomic-0-1",
                        "atomic-1-0",
                        "atomic-1-1"
                  },
                  {false, true, true, true, true, true, true},
                  {0, 4, 4, 4, 4, 4, 4},
                  {
                          { -999, -999}, // ルートノードはメッシュを持ちません
                          {0, 0},
                          {0, 0},
                          {0, 0},
                          {0, 0},
                          {0, 0},
                          {0, 0}
                  },
                  {
                          {CityObjectList()},
                          {{{{0, 0}, "primary-0"}}},
                          {{{{0, 0}, "primary-1"}}},
                          {{{{0, 0}, "atomic-0-0"}}},
                          {{{{0, 0}, "atomic-0-1"}}},
                          {{{{0, 0}, "atomic-1-0"}}},
                          {{{{0, 0}, "atomic-1-1"}}}
                  }
    );
}
