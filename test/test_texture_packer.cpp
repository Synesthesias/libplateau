#include "gtest/gtest.h"
#include <plateau/polygon_mesh/model.h>
#include <plateau/texture/texture_packer.h>
#include <filesystem>

using namespace plateau::polygonMesh;
using namespace plateau::texture;
namespace fs = std::filesystem;

class  TexturePackerTest : public ::testing::Test {
public:
    static constexpr int rectangle_count = 16; // テストデータでは、板ポリゴンをこの枚数だけ生成します。
    static constexpr int pack_texture_width = 513;
    static constexpr int pack_texture_height = 513;
    static const fs::path texture_dir;
    static const std::vector<std::string> texture_files_each_different;
    static const std::vector<std::string> expect_files;
};

const fs::path TexturePackerTest::texture_dir = fs::u8path(u8"../data/日本語パステスト/test_texture_packer");

const std::vector<std::string> TexturePackerTest::texture_files_each_different = {
"test_image_0.png", "test_image_1.png", "test_image_2.jpg", "test_image_3.jpg",
"test_image_4.tif", "test_image_5.tif", "test_image_6.png", "test_image_7.png",
"test_image_8.jpg", "test_image_9.jpg", "test_image_10.tif", "test_image_11.tif",
"test_image_12.png", "test_image_13.png", "test_image_14.jpg", "test_image_15.jpg"
};

const std::vector<std::string> TexturePackerTest::expect_files = {
"packed_image_test_image_0_000000.png",
"packed_image_test_image_4_000000.png",
"packed_image_test_image_8_000000.png",
"packed_image_test_image_12_000000.png"
};

namespace {

    Node createTestNode(const std::string& node_name, const std::vector<std::string>& texture_files) {
        auto node = Node(node_name);
        auto mesh = Mesh();
        TVec3d base_pos = {0, 0, 0};
        unsigned int base_id = 0;
        for(int i=0; i < TexturePackerTest::rectangle_count; i++) {
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

            auto texture_path = fs::absolute(fs::path(TexturePackerTest::texture_dir) / fs::u8path(texture_files.at(i)).make_preferred());

            mesh.addVerticesList(vertices);
            mesh.addIndicesList(indices, 0, false);
            mesh.addUV1(uv1, vertices.size());
            mesh.addSubMesh(texture_path.u8string(), nullptr, mesh.getIndices().size() - 6, mesh.getIndices().size() - 1);

            base_pos = base_pos + TVec3d{2, 0, 0};
            base_id += 4;
        }
        node.setMesh(std::make_unique<Mesh>(mesh));
        return node;
    }

    /// テスト用のModelを作って返します。
    /// 1つのMeshの中に16枚の板があり、それぞれの板には異なるテクスチャが貼られているモデルを作って返します。
    Model createTestModel(const std::vector<std::string>& texture_files) {
        auto model = Model();
        auto node = createTestNode("node", texture_files);
        model.addNode(std::move(node));
        return model;

    }

    /// 上のcreateTestModelで生成したModelに対してTexturePacker.processを実行した結果のUV1が想定通りかをチェックします。
    bool isUvOK(Model& model){
        std::vector<TVec2f> expect_uv;
        static const std::vector<TVec2f> rectangle_uv = {
                {0, 0.5}, {0,1}, {0.5, 1}, {0.5, 0.5}, // 1枚目のテクスチャが、アトラステクスチャの左上にパックされる
                {0.5, 0.5}, {0.5, 1}, {1, 1}, {1, 0.5}, // 2枚目が右上に
                { 0, 0}, {0, 0.5}, {0.5, 0.5}, {0.5, 0}, // 3枚目が左下に
                {0.5, 0}, {0.5, 0.5}, {1, 0.5}, {1, 0} // 4枚目が右下に
        };
        for(int i=0; i < TexturePackerTest::rectangle_count / 4; i++){
            expect_uv.insert(expect_uv.end(), rectangle_uv.begin(), rectangle_uv.end());
        }
        const auto& uv = model.getRootNodeAt(0).getMesh()->getUV1();
        const auto count = uv.size();
        for(int i=0; i<count; i++) {
            const auto diff = uv[i] - expect_uv[i];
            const auto len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if(len > 0.01){
                return false;
            }
        }
        return true;
    }

    void deletePackedTextures(Model& model) {
        auto node_count = model.getRootNodeCount();
        for(int n=0; n<node_count; n++) {
            const auto& sub_meshes = model.getRootNodeAt(n).getMesh()->getSubMeshes();
            auto count = sub_meshes.size();
            for(int i=0; i<count; i++){
                const auto& texture_path = fs::u8path(sub_meshes.at(i).getTexturePath());
                if(texture_path.empty()) continue;
                fs::remove(texture_path);
            }
        }

    }

    void packTestData(Model& model) {
        auto packer = TexturePacker(TexturePackerTest::pack_texture_width, TexturePackerTest::pack_texture_height);
        packer.process(model);
    };
}

TEST_F(TexturePackerTest, isUvOK) {  // NOLINT
    auto model = createTestModel(TexturePackerTest::texture_files_each_different);
    packTestData(model);
    EXPECT_TRUE(isUvOK(model));
    deletePackedTextures(model);
}

void expectImageFileExist(std::vector<std::string> expect_files, size_t expect_height, size_t expect_width) {
    // テクスチャパック後には、パックしたテクスチャが存在します
    for(const auto& file_name : expect_files) {
        const auto path = fs::path(TexturePackerTest::texture_dir) / fs::u8path(file_name);
        EXPECT_TRUE(fs::exists(path));

        // 出力ファイルを画像としてロードできるかチェックします。
        bool load_result = false;
        auto tex = TextureImageBase::tryCreateFromFile(path.u8string(), 999, load_result);
        EXPECT_TRUE(load_result);
        EXPECT_TRUE(tex->getHeight() == expect_height);
        EXPECT_TRUE(tex->getWidth() == expect_width);
    }
}

TEST_F(TexturePackerTest, packGeneratesTextures) {

    // テクスチャパック前には、パックしたテクスチャは存在しません
    for(const auto& file_name : expect_files) {
        const auto path = fs::path(TexturePackerTest::texture_dir) / fs::u8path(file_name);
        EXPECT_FALSE(fs::exists(path));
    }

    auto model = createTestModel(TexturePackerTest::texture_files_each_different);
    packTestData(model);

    expectImageFileExist(expect_files, pack_texture_height, pack_texture_width);

    deletePackedTextures(model);
}

TEST_F(TexturePackerTest, canHandleMultipleNodesWithSameTextures) {
    auto node1 = createTestNode("node1", TexturePackerTest::texture_files_each_different);
    auto node2 = createTestNode("node2", TexturePackerTest::texture_files_each_different);
    auto model = Model();
    model.addNode(std::move(node1));
    model.addNode(std::move(node2));
    packTestData(model);

    expectImageFileExist(expect_files, pack_texture_height, pack_texture_width);
    auto file_count = std::distance(fs::directory_iterator(fs::path(texture_dir)), fs::directory_iterator{});
    EXPECT_EQ(file_count, 20); // 入力画像が16枚、出力画像が4枚。
    deletePackedTextures(model);
}
