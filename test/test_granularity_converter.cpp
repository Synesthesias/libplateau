#include "gtest/gtest.h"
#include "plateau/polygon_mesh/model.h"
#include <plateau/granularity_convert/granularity_converter.h>

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
                auto city_obj_index = city_object_indices.at(j);
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
        auto node = Node("node");
        node.setMesh(std::move(mesh));
        auto model = Model();
        model.addNode(std::move(node));
        return model;
    }
}

TEST_F(GranularityConverterTest, convertFromAreaToAtomic) { // NOLINT
    auto area_model = createTestModelOfAreaGranularity();
    auto option = GranularityConvertOption(MeshGranularity::PerAtomicFeatureObject, 10);
    auto converter = GranularityConverter();
    auto atomic_model = converter.convert(area_model, option);

    ASSERT_EQ(atomic_model.getRootNodeCount(), 1);
    const auto& root_node = atomic_model.getRootNodeAt(0);

    const auto& first_primary = root_node.getChildAt(0);
    ASSERT_EQ(first_primary.getChildCount(), 2);
    const auto& first_atomic = first_primary.getChildAt(0);
    ASSERT_TRUE(first_atomic.getMesh() != nullptr);
    const auto& first_atomic_mesh = first_atomic.getMesh();
    ASSERT_EQ(first_atomic_mesh->getVertices().size(), 4);
    const auto first_uv4 = first_atomic_mesh->getUV4().at(0);
    EXPECT_TRUE((int)first_uv4.x == 0);
    EXPECT_TRUE((int)first_uv4.y == 1);
}
