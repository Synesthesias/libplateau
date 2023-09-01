
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

struct jpeg_error_mgr;

namespace plateau::texture {
    class TiffTextureImage {
    public:

        typedef enum {
            NONE_COMPRESSION, LZW_COMPRESSION
        } COMPRESSION_TYPE_t;

        explicit TiffTextureImage(const std::string& file_name) :
                load_succeed(init(file_name))
        {
            if(!load_succeed) throw std::runtime_error("tiff load failed.");
        }

        size_t getWidth() const {
            return image_width;
        }
        size_t getHeight() const {
            return image_height;
        }
        std::vector<std::vector<uint8_t>>& getBitmapData();

    private:
        bool init(const std::string& fileName);
        std::vector<std::vector<uint8_t>> bitmapData;
        unsigned int image_width = 0;
        unsigned int image_height = 0;
        uint16_t image_channels = 0;
        bool load_succeed;
    };
} // namespace tiff


