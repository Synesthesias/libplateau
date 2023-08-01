
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct jpeg_error_mgr;

namespace plateau::texture {
    class JpegTextureImage {
    public:
        explicit JpegTextureImage() {
        }

        /**
         * \brief 指定されたファイルから画像を読み込み、テクスチャ画像を作成します。
         * \param file_name 画像ファイルのパス
         * \param height_limit 画像の高さがこの値を超える場合画像データは読み込まれません。
         * \return 読み込みに成功した場合true、それ以外はfalse
         */
        bool init(const std::string& file_name, const size_t height_limit);
        void init(size_t width, size_t height, size_t color);
        ~JpegTextureImage() {
        }

        size_t getWidth() const {
            return image_width;
        }
        size_t getHeight() const {
            return image_height;
        }
        std::vector<uint8_t>& getBitmapData() {
            return bitmapData;
        }

        bool save(const std::string& fileName);
        void pack(size_t x_delta, size_t y_delta, const JpegTextureImage& image);

    private:
        std::shared_ptr<::jpeg_error_mgr> jpegErrorManager;
        std::vector<uint8_t> bitmapData;
        std::string                       filePath;
        size_t                            image_width;
        size_t                            image_height;
        size_t                            image_channels;
        int                               colourSpace;
    };

} // namespace plateau::texture
