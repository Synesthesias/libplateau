#pragma once

#include <libplateau_api.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct jpeg_error_mgr;

namespace plateau::texture {

    class LIBPLATEAU_EXPORT TextureImage {
    public:

        explicit TextureImage(const std::string& fileName);
        explicit TextureImage(size_t width, size_t height, size_t color);
        ~TextureImage();

        size_t getWidth() const {
            return width;
        }
        size_t getHeight() const {
            return height;
        }

        void save(const std::string& fileName) const;
        void pack(size_t xdelta, size_t ydelta, const TextureImage& image);

    private:
        std::shared_ptr<::jpeg_error_mgr> jpegErrorManager;
        std::vector<std::vector<uint8_t>> bitmapData;
        std::string                       filePath;
        size_t                            width;
        size_t                            height;
        size_t                            channels;
        int                               colourSpace;
    };

}
