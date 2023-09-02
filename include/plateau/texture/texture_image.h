//
//#pragma once
//
//#include <string>
//#include <filesystem>
//#include <optional>
//
//#include <plateau/texture/jpeg_texture_image.h>
//#include <plateau/texture/png_texture_image.h>
//#include <plateau/texture/tiff_texture_image.h>
//#include <libplateau_api.h>
//
//namespace plateau::texture {
//    class LIBPLATEAU_EXPORT TextureImage {
//    public:
//        enum class TextureType {
//            None, Jpeg, Png, Tiff
//        };
//
//        explicit TextureImage(size_t width, size_t height);
//
//        size_t getWidth() const {
//            return image_width_;
//        }
//
//        size_t getHeight() const {
//            return image_height_;
//        }
//
//        TextureType getTextureType() const {
//            return texture_type_;
//        }
//
//        std::string getImageFilePath() {
//            return image_file_path_;
//        }
//
//
//        void save(const std::string& file_name);
//
//    private:
//        static constexpr unsigned char DefaultColor = 80;
//        TextureType texture_type_;
//        std::optional<JpegTextureImage> jpeg_image_;
//        std::optional<PngTextureImage> png_image_;
//        std::optional<TiffTextureImage> tiff_image_;
//        size_t image_width_;
//        size_t image_height_;
//        std::string image_file_path_;
//    };
//}
