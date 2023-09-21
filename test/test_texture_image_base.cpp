#include "gtest/gtest.h"
#include <filesystem>
#include <plateau/texture/texture_image_base.h>

namespace fs = std::filesystem;
using namespace plateau::texture;

class TextureImageTest : public ::testing::Test {
public:
    static const fs::path test_dir;
};

const fs::path TextureImageTest::test_dir = fs::u8path(u8"../data/日本語パステスト/test_images");


TEST_F(TextureImageTest, canReadImageFiles) {
    std::vector<std::string> files_to_read = {
            "test_image.jpeg", "test_image.jpg", "test_image.png",
            "test_image.tif", "test_image.tiff",
            "test_map.png"
    };
    for(const auto& file_name : files_to_read) {
        const auto path = fs::path(TextureImageTest::test_dir) / file_name;
        bool result = false;
        const auto image = TextureImageBase::tryCreateFromFile(path.u8string(), 9999, result);
        EXPECT_TRUE(result) << "Loading of " << path.u8string() << " should succeed.";
        EXPECT_TRUE(image->getHeight() > 0);
        EXPECT_TRUE(image->getWidth() > 0);
    }
}
