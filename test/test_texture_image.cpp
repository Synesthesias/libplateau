//#include <gtest/gtest.h>
//#include <plateau/texture/texture_image.h>
//
//using namespace plateau::texture;
//namespace fs = std::filesystem;
//class TextureImageTest : public ::testing::Test {
//
//protected:
//    void SetUp() override {
//    }
//public:
//    static constexpr std::string_view test_data_dir = "../data/日本語パステスト/test_images/";
//};
//
//namespace {
//    TextureImage::TextureType getTextureTypeOf(const std::string& file_name) {
//        auto dir = fs::absolute(fs::u8path(TextureImageTest::test_data_dir));
//        auto texture = TextureImage((fs::path(dir) / file_name).u8string(), 256);
//        return texture.getTextureType();
//    }
//}
//
//TEST_F(TextureImageTest, getTextureType_Returns_Its_Extension) { // NOLINT
//    ASSERT_EQ(getTextureTypeOf("test_image.tif"), TextureImage::TextureType::Tiff);
//    ASSERT_EQ(getTextureTypeOf("test_image.tiff"), TextureImage::TextureType::Tiff);
//    ASSERT_EQ(getTextureTypeOf("test_image.jpg"), TextureImage::TextureType::Jpeg);
//    ASSERT_EQ(getTextureTypeOf("test_image.jpeg"), TextureImage::TextureType::Jpeg);
//    ASSERT_EQ(getTextureTypeOf("test_image.png"), TextureImage::TextureType::Png);
//}
