#include "gtest/gtest.h"
#include <plateau/polygon_mesh/model.h>
#include <plateau/texture/texture_packer.h>
#include <filesystem>

using namespace plateau::polygonMesh;
using namespace plateau::texture;
namespace fs = std::filesystem;

class TexturePackerTest : public ::testing::Test {
protected:

};

namespace {
    /// テスト用のModelを作って返します。
    /// 1つのMeshの中に16枚の板があり、それぞれの板には異なるテクスチャが貼られているモデルを作って返します。
    Model createTestModel() {
        auto model = Model();
        auto node = Node("node");
        auto mesh = Mesh();

        // 正方形ポリゴンを複数作ります。
        static const std::vector<std::string> texture_files = {
                "test_image_0.png", "test_image_1.png", "test_image_2.jpg", "test_image_3.jpg",
                "test_image_4.tif", "test_image_5.tif", "test_image_6.png", "test_image_7.png",
                "test_image_8.jpg", "test_image_9.jpg", "test_image_10.tif", "test_image_11.tif",
                "test_image_12.png", "test_image_13.png", "test_image_14.jpg", "test_image_16.jpg"
        };
        static const auto texture_dir = fs::u8path(u8"./data/日本語パステスト/test_texture_packer");
        TVec3d base_pos = {0, 0, 0};
        unsigned int base_id = 0;
        for(int i=0; i<16; i++) {
            auto vertices = std::vector<TVec3d> {
                base_pos,
                base_pos + TVec3d{0, 1, 0},
                base_pos + TVec3d{1, 1, 0},
                base_pos + TVec3d{1, 0, 0}
            };

            auto indices = std::vector<unsigned int> {
                base_id, base_id + 1, base_id + 2,
                base_id, base_id + 2, base_id + 3
            };

            auto uv1 = std::vector<TVec2f> {
                TVec2f{0, 0}, TVec2f{0, 1}, TVec2f{1, 1}, TVec2f{1, 0}
            };

            auto texture_path = fs::absolute(fs::path(texture_dir) / fs::u8path(texture_files.at(i)).make_preferred());

            mesh.addVerticesList(vertices);
            mesh.addIndicesList(indices, 0, false);
            mesh.addUV1(uv1, vertices.size());
            mesh.addSubMesh(texture_path.u8string(), mesh.getIndices().size() - 6, mesh.getIndices().size() - 1);

            base_pos = base_pos + TVec3d{2, 0, 0};
            base_id += 4;
        }
        node.setMesh(std::make_unique<Mesh>(mesh));
        model.addNode(std::move(node));
        return model;

    }
}

TEST_F(TexturePackerTest, foobarTest) {  // NOLINT // TODO テストに名前をつける
    auto model = createTestModel();
    auto packer = TexturePacker(513, 513, 10);
    packer.process(model);
}
