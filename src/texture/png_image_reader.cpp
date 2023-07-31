
#include <plateau/texture/png_image_reader.h>
#include <plateau/texture/jpeg_image_reader.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#define SIGNATURE_NUM 8

namespace png
{
    png_structp png_ptr;
    png_infop info_ptr;
    size_t number_of_passes;
    png_bytep* row_pointers;

    PngTextureImage::PngTextureImage() : image_width(0), image_height(0), image_channels(0) {

    }

    std::vector<std::vector<uint8_t>>&
    PngTextureImage::getBitmapData() {
        return bitmapData;
    }

    bool
    PngTextureImage::init(const std::string& fileName) {
        FILE* fi;
        int j;
        unsigned int width, height;
        unsigned int readSize;

        png_structp png;
        png_infop info;
        png_bytepp datap;
        png_byte type;
        png_byte signature[8];

        fi = fopen(fileName.c_str(), "rb");
        if (fi == NULL) {
            return -1;
        }

        readSize = fread(signature, 1, SIGNATURE_NUM, fi);

        if (png_sig_cmp(signature, 0, SIGNATURE_NUM)) {
            fclose(fi);
            return -1;
        }

        png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (png == NULL) {
            fclose(fi);
            return -1;
        }

        info = png_create_info_struct(png);
        if (info == NULL) {
            png_destroy_read_struct(&png, NULL, NULL);
            fclose(fi);
            return -1;
        }

        png_init_io(png, fi);
        png_set_sig_bytes(png, readSize);
        png_read_png(png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, NULL);

        width = png_get_image_width(png, info);
        height = png_get_image_height(png, info);

        datap = png_get_rows(png, info);

        type = png_get_color_type(png, info);

        if (type != PNG_COLOR_TYPE_RGB) {
            png_destroy_read_struct(&png, &info, NULL);
            fclose(fi);
            return -1;
        }

        image_width = width;
        image_height = height;
        if (type == PNG_COLOR_TYPE_RGB) {
            image_channels = 3;
        }

        size_t row_stride = image_width * image_channels;

        bitmapData.clear();
        bitmapData.reserve(image_height);

        for (j = 0; j < image_height; j++) {

            std::vector<uint8_t> vec(row_stride);
            uint8_t* p = vec.data();

            memcpy(p, datap[j], image_width * image_channels);
            bitmapData.push_back(vec);
        }

        png_destroy_read_struct(&png, &info, NULL);
        fclose(fi);

        return 0;
    }

    void
    PngTextureImage::pack(size_t xdelta, size_t ydelta, const PngTextureImage& image, JpegTextureImage& targetImage) {
        auto srcHeight = image.getHeight();

        auto fromPtr = image.bitmapData.data();
        auto toPtr = targetImage.getBitmapData().data();

        for (auto y = 0; y < srcHeight; ++y) {
            std::copy(fromPtr[y].begin(), fromPtr[y].end(), toPtr[ydelta + y].begin() + xdelta * 3);
        }
    }
} // namespace png

