
#pragma once


#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "png.h"

//#include "png.h"
//#include "jpeg.hpp"

//using namespace jpeg;

namespace png
{
    class PngTextureImage
    {
    public:

        explicit PngTextureImage() {}
    
        void init(const std::string& fileName);
        //explicit TextureImage(size_t width, size_t height, size_t color);
        ~PngTextureImage() {}

        size_t getWidth() const { return width; }
        size_t getHeight() const { return height; }
        std::vector<std::vector<uint8_t>>& getBitmapData() { return bitmapData; }

        void save(const std::string& fileName) const;
        //void pack(size_t xdelta, size_t ydelta, const PngTextureImage& image, JpegTextureImage& targetImage);

    private:
        std::vector<std::vector<uint8_t>> bitmapData;
        //std::string                       filePath;
        //size_t                            width;
        //size_t                            height;
        //size_t                            channels;
        //int                               colourSpace;

        size_t x;
        size_t y;
        size_t width;
        size_t height;
        png_byte color_type;
        png_byte bit_depth;

        png_structp png_ptr;
        png_infop info_ptr;
        size_t number_of_passes;
        png_bytep* row_pointers;
    };

} // namespace jpeg


