
#include <plateau/texture/texture_image.h>

#include <string>
#include <filesystem>

namespace plateau::texture {
    namespace fs = std::filesystem;
    TextureImage::TextureImage(const std::string& file_name, const size_t height_limit) {
        reset();

        image_file_path_ = file_name;
        auto extension = fs::u8path(file_name).extension().u8string();

        if (extension == ".jpg" || extension == ".jpeg") {
            texture_type_ = TextureType::Jpeg;
            jpeg_image_.init(file_name, height_limit);

            image_width_ = jpeg_image_.getWidth();
            image_height_ = jpeg_image_.getHeight();
        } else if (extension == ".tif" || extension == ".tiff") {
            texture_type_ = TextureType::Tiff;
            tiff_image_.init(file_name);

            image_width_ = tiff_image_.getWidth();
            image_height_ = tiff_image_.getHeight();
        } else if (extension == ".png") {
            texture_type_ = TextureType::Png;
            png_image_.init(file_name);

            image_width_ = png_image_.getWidth();
            image_height_ = png_image_.getHeight();
        }
    }
    
    TextureImage::TextureImage(const size_t width, const size_t height, const size_t gray) {
        texture_type_ = TextureType::Jpeg;
        jpeg_image_.init(width, height, gray);
    }

    void TextureImage::init(size_t width, size_t height, unsigned char gray) {
        image_width_ = width;
        image_height_ = height;
        image_color_ = gray;
        jpeg_image_.init(width, height, gray);
    }

    void
        TextureImage::reset() {
        image_width_ = 0;
        image_height_ = 0;
        image_color_ = 0;
        texture_type_ = TextureType::None;
    }

    // 指定されたファイル名で、jpegファイルを保存
    void TextureImage::save(const std::string& file_name) {
        jpeg_image_.save(file_name);
    }

    // 指定された座標（xdelta、ydelta）にimageの画像を転送
    void TextureImage::pack(const size_t x_delta, const size_t y_delta, const TextureImage& image) {
        if (image.texture_type_ == TextureType::Jpeg) {
            jpeg_image_.pack(x_delta, y_delta, image.jpeg_image_);
        } else if (image.texture_type_ == TextureType::Tiff) {

            auto pimage = image.tiff_image_;
            pack(x_delta, y_delta, pimage, jpeg_image_);
        } else if (image.texture_type_ == TextureType::Png) {
            auto pimage = image.png_image_;
            pack(x_delta, y_delta, pimage, jpeg_image_);
        }
    }
    
    void TextureImage::pack(const size_t x_delta, const size_t y_delta, TiffTextureImage& image, JpegTextureImage& target_image) {
        auto srcHeight = image.getHeight();

        const auto from_vector = image.getBitmapData().data();
        auto& to = target_image.getBitmapData();

        for (auto y = 0; y < srcHeight; ++y) {
            std::copy(from_vector[y].begin(), from_vector[y].end(), to.begin() + (y + y_delta) * target_image.getWidth() * 3 + x_delta * 3);
        }
    }

    void
        TextureImage::pack(size_t x_delta, size_t y_delta, PngTextureImage& image, JpegTextureImage& target_image) {
        auto srcHeight = image.getHeight();

        const auto from_vector = image.getBitmapData().data();
        auto& to = target_image.getBitmapData();

        for (auto y = 0; y < srcHeight; ++y) {
            std::copy(from_vector[y].begin(), from_vector[y].end(), to.begin() + (y + y_delta) * target_image.getWidth() * 3 + x_delta * 3);
        }
    }
}
