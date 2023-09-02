//
//#include <plateau/texture/texture_image.h>
//
//#include <string>
//#include <filesystem>
//#include <iostream>
//
//namespace plateau::texture {
//    namespace fs = std::filesystem;
//
//    TextureImage::TextureImage(const size_t width, const size_t height) :
//            image_width_(width), image_height_(height)
//    {
//        texture_type_ = TextureType::Jpeg;
//        jpeg_image_ = JpegTextureImage(width, height, DefaultColor);
//    }
//
//
//    // 指定されたファイル名で、jpegファイルを保存
//    void TextureImage::save(const std::string& file_name) {
//        jpeg_image_.value().save(file_name);
//    }
//
//}
