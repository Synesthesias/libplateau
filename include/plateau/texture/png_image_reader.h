
#pragma once

#include <plateau/texture/jpeg_image_reader.h>

#include <cstdint>
#include <string>
#include <vector>

namespace plateau::texture {
    class PngTextureImage {
    public:

        explicit PngTextureImage();

        bool init(const std::string& fileName);
        ~PngTextureImage() {
        }

        size_t getWidth() const {
            return image_width;
        }
        size_t getHeight() const {
            return image_height;
        }
        std::vector<std::vector<uint8_t>>& getBitmapData();

        void save(const std::string& fileName) const;
        void pack(size_t xdelta, size_t ydelta, const PngTextureImage& image, JpegTextureImage& targetImage);

    private:
        std::vector<std::vector<uint8_t>> bitmapData;
        unsigned int image_width;
        unsigned int image_height;
        unsigned int image_channels;
    };
} // namespace png


