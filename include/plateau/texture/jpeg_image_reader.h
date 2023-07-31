
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct jpeg_error_mgr;

namespace jpeg
{

    class JpegTextureImage
    {
    public:

        explicit JpegTextureImage() {}

        bool init(const std::string& fileName);
        void init(size_t width, size_t height, size_t color);
        ~JpegTextureImage() {}

        size_t getWidth() const { return image_width; }
        size_t getHeight() const { return image_height; }
        std::vector<std::vector<uint8_t>>& getBitmapData() { return bitmapData; }

        bool save(const std::string& fileName) const;
        void pack(size_t xdelta, size_t ydelta, const JpegTextureImage& image, JpegTextureImage& targetImage);

    private:
        std::shared_ptr<::jpeg_error_mgr> jpegErrorManager;
        std::vector<std::vector<uint8_t>> bitmapData;
        std::string                       filePath;
        size_t                            image_width;
        size_t                            image_height;
        size_t                            image_channels;
        int                               colourSpace;
    };

} // namespace jpeg


