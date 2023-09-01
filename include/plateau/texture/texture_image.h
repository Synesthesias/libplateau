
#pragma once

#include <string>
#include <filesystem>
#include <optional>

#include <plateau/texture/jpeg_texture_image.h>
#include <plateau/texture/png_texture_image.h>
#include <plateau/texture/tiff_texture_image.h>
#include <libplateau_api.h>

namespace plateau::texture {
    class LIBPLATEAU_EXPORT TextureImage {
    public:
        enum class TextureType {
            None, Jpeg, Png, Tiff
        };

        /**
         * \brief 指定されたファイルから画像を読み込み、テクスチャ画像を作成します。
         * \param file_name 画像ファイルのパス
         * \param height_limit 画像の高さがこの値を超える場合画像データは読み込まれません。
         */
        explicit TextureImage(const std::string& file_name, size_t height_limit);
        explicit TextureImage(size_t width, size_t height);

        void reset();

        size_t getWidth() const {
            return image_width_;
        }

        size_t getHeight() const {
            return image_height_;
        }

        TextureType getTextureType() const {
            return texture_type_;
        }

        std::string getImageFilePath() {
            return image_file_path_;
        }


        void save(const std::string& file_name);
        void pack(size_t x_delta, size_t y_delta, const TextureImage& image);
        void pack(size_t x_delta, size_t y_delta, TiffTextureImage& image, JpegTextureImage& target_image);
        void pack(size_t x_delta, size_t y_delta, PngTextureImage& image, JpegTextureImage& target_image);

    private:
        static constexpr unsigned char DefaultColor = 80;
        TextureType texture_type_;
        std::optional<JpegTextureImage> jpeg_image_;
        std::optional<PngTextureImage> png_image_;
        std::optional<TiffTextureImage> tiff_image_;
        size_t image_width_;
        size_t image_height_;
        std::string image_file_path_;
    };
}
