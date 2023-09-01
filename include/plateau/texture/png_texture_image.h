
#pragma once

#include <plateau/texture/png_texture_image.h>

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

namespace plateau::texture {
    class PngTextureImage {
    public:
        explicit PngTextureImage(const std::string& file_name) :
        load_succeed(init(file_name))
        {
            if(!load_succeed) throw std::runtime_error("png load failed.");
        };

        size_t getWidth() const {
            return image_width_;
        }

        size_t getHeight() const {
            return image_height_;
        }

        bool loadSucceed() const {
            return load_succeed;
        }

        std::vector<std::vector<uint8_t>>& getBitmapData();

    private:
        bool init(const std::string& file_name);
        std::vector<std::vector<uint8_t>> bitmap_data_;
        unsigned int image_width_ = 0;
        unsigned int image_height_ = 0;
        unsigned int image_channels_ = 0;
        bool load_succeed;
    };
}
