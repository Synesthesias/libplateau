
#include <plateau/texture/texture_image.h>

#include <string>
#include <filesystem>
#include <iostream>

namespace plateau::texture {
    namespace fs = std::filesystem;
    TextureImage::TextureImage(const std::string& file_name, const size_t height_limit) {

        image_file_path_ = file_name;
        auto extension = fs::u8path(file_name).extension().u8string();

        if (extension == ".jpg" || extension == ".jpeg") {
            texture_type_ = TextureType::Jpeg;
            jpeg_image_= JpegTextureImage(file_name, height_limit);

            image_width_ = jpeg_image_.value().getWidth();
            image_height_ = jpeg_image_.value().getHeight();
            return;
        } else if (extension == ".tif" || extension == ".tiff") {
            texture_type_ = TextureType::Tiff;
            tiff_image_ = TiffTextureImage(file_name);

            image_width_ = tiff_image_.value().getWidth();
            image_height_ = tiff_image_.value().getHeight();
            return;
        } else if (extension == ".png") {
            texture_type_ = TextureType::Png;
            png_image_ = PngTextureImage(file_name);
            bool result = png_image_.value().loadSucceed();
            if(!result) std::cerr << "Failed to load png image. file = " << file_name << std::endl;

            image_width_ = png_image_.value().getWidth();
            image_height_ = png_image_.value().getHeight();
            return;
        }
        throw std::runtime_error("unknown image extension.");
    }
    
    TextureImage::TextureImage(const size_t width, const size_t height) :
            image_width_(width), image_height_(height)
    {
        texture_type_ = TextureType::Jpeg;
        jpeg_image_ = JpegTextureImage(width, height, DefaultColor);
    }


    // 指定されたファイル名で、jpegファイルを保存
    void TextureImage::save(const std::string& file_name) {
        jpeg_image_.value().save(file_name);
    }

    // 指定された座標（xdelta、ydelta）にimageの画像を転送
    void TextureImage::pack(const size_t x_delta, const size_t y_delta, const TextureImage& image) {
        if (image.texture_type_ == TextureType::Jpeg) {
            jpeg_image_.value().pack(x_delta, y_delta, image.jpeg_image_.value());
        } else if (image.texture_type_ == TextureType::Tiff) {

            auto pimage = image.tiff_image_;
            pack(x_delta, y_delta, pimage.value(), jpeg_image_.value());
        } else if (image.texture_type_ == TextureType::Png) {
            auto pimage = image.png_image_;
            pack(x_delta, y_delta, pimage.value(), jpeg_image_.value());
        }else {
            throw std::runtime_error("unknown texture type.");
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
