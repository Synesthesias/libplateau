#include "gtest/gtest.h"
#include "plateau/polygon_mesh/node.h"
#include "plateau/polygon_mesh/model.h"
#include "plateau/polygon_mesh/map_attacher.h"
#include <plateau/polygon_mesh/mesh.h>

using namespace plateau::polygonMesh;
namespace fs = std::filesystem;

class MapAttacherTest : public ::testing::Test {

};

TEST_F(MapAttacherTest, AttachGeneratesImageFiles) { // NOLINT
    const auto test_dir = fs::u8path(u8"../data/日本語パステスト/map_attacher_text");
    fs::remove_all(test_dir);
    fs::create_directories(test_dir);
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
    const auto geo_ref = GeoReference(9);

    MapAttacher().attach(model,
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

    fs::remove_all(test_dir);
}
