
#include <plateau/texture/png_texture_image.h>
#include <plateau/texture/jpeg_texture_image.h>

#include <cstdio>
#include <string>
#include <cstdarg>

#include "png.h"

namespace plateau::texture {
    png_structp png_ptr;
    png_infop info_ptr;
    size_t number_of_passes;
    png_bytep* row_pointers;

    PngTextureImage::PngTextureImage() : image_width_(0), image_height_(0), image_channels_(0) {
    }

    std::vector<std::vector<uint8_t>>& PngTextureImage::getBitmapData() {
        return bitmap_data_;
    }

    bool PngTextureImage::init(const std::string& file_name) {
        png_structp png;
        png_infop info;
        png_byte signature[8];

        FILE* fi = fopen(file_name.c_str(), "rb");
        if (fi == nullptr) {
            return false;
        }

        const unsigned int read_size = fread(signature, 1, 8, fi);

        if (png_sig_cmp(signature, 0, 8)) {
            fclose(fi);
            return false;
        }

        png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (png == nullptr) {
            fclose(fi);
            return false;
        }

        info = png_create_info_struct(png);
        if (info == nullptr) {
            png_destroy_read_struct(&png, nullptr, nullptr);
            fclose(fi);
            return false;
        }

        png_init_io(png, fi);
        png_set_sig_bytes(png, read_size);
        png_read_png(png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, nullptr);

        const unsigned int width = png_get_image_width(png, info);
        const unsigned int height = png_get_image_height(png, info);
        const png_byte type = png_get_color_type(png, info);

        const png_bytepp datap = png_get_rows(png, info);

        if (type != PNG_COLOR_TYPE_RGB) {
            png_destroy_read_struct(&png, &info, nullptr);
            fclose(fi);
            return false;
        }

        image_width_ = width;
        image_height_ = height;
        if (type == PNG_COLOR_TYPE_RGB) {
            image_channels_ = 3;
        }

        const size_t row_stride = image_width_ * image_channels_;

        bitmap_data_.clear();
        bitmap_data_.reserve(image_height_);

        for (int j = 0; j < image_height_; j++) {

            std::vector<uint8_t> vec(row_stride);
            uint8_t* p = vec.data();

            memcpy(p, datap[j], image_width_ * image_channels_);
            bitmap_data_.push_back(vec);
        }

        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fi);

        return true;
    }
}
