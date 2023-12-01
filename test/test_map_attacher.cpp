#include "gtest/gtest.h"
#include "plateau/polygon_mesh/node.h"
#include "plateau/polygon_mesh/model.h"
#include "plateau/polygon_mesh/map_attacher.h"
#include <plateau/polygon_mesh/mesh.h>

using namespace plateau::polygonMesh;
namespace fs = std::filesystem;

class MapAttacherTest : public ::testing::Test {

};

namespace {
    Model createTestModel(){
        auto mesh = Mesh();
        mesh.addVerticesList({
                                     {0, 0, 0}, {0, 500, 0}, {500, 500, 0}, {500, 0, 0}
                             });
        mesh.addIndicesList({
                                    0, 1, 2, 0, 2, 3
                            }, 0, false);
        auto node = Node("node");
        node.setMesh(std::make_unique<Mesh>(mesh));
        auto model = Model();
        model.addNode(std::move(node));
        return model;
    }
}

TEST_F(MapAttacherTest, AttachGeneratesImageFiles) { // NOLINT
    const auto test_dir = fs::u8path(u8"../data/日本語パステスト/test_map_attacher");
    fs::remove_all(test_dir);
    fs::create_directories(test_dir);
    auto model = createTestModel();
    const auto geo_ref = GeoReference(9);

    int success_count = MapAttacher().attach(model,
                         "https://cyberjapandata.gsi.go.jp/xyz/seamlessphoto/{z}/{x}/{y}.jpg",
                         test_dir,
                         16, geo_ref);

    const static std::vector<std::string> expect_files = {
            "16/58223/25734.jpg", "16/58223/25735.jpg",
            "16/58224/25734.jpg", "16/58224/25735.jpg"
    };
    for(const auto& expect_file : expect_files) {
        const auto path = (fs::path(test_dir) / expect_file).make_preferred();
        EXPECT_TRUE(fs::exists(path)) << path.u8string() << " exists";
    }
    EXPECT_TRUE(fs::exists(fs::u8path(u8"../data/日本語パステスト/test_map_attacher/combined_map_mesh0_v0_p0.png")));
    EXPECT_EQ(success_count, 4);
    fs::remove_all(test_dir);
}

TEST_F(MapAttacherTest, CanLoadPngMapOfGSI) { // NOLINT
    const auto test_dir = fs::u8path(u8"../data/日本語パステスト/test_map_attacher2");
    fs::remove_all(test_dir);
    fs::create_directories(test_dir);
    auto model = createTestModel();
    const auto geo_ref = GeoReference(9);

    int success_count = MapAttacher().attach(model,
                         "https://cyberjapandata.gsi.go.jp/xyz/std/{z}/{x}/{y}.png",
                         test_dir,
                         16, geo_ref);
    EXPECT_EQ(success_count, 4);
    fs::remove_all(test_dir);
}
