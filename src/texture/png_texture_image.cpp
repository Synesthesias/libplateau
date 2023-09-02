
#include <plateau/texture/png_texture_image.h>
#include <plateau/texture/jpeg_texture_image.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <cstdarg>
#include <filesystem>
#include <iostream>

#include "png.h"

namespace plateau::texture {


    const std::vector<std::vector<uint8_t>>& PngTextureImage::getBitmapData() const {
        return bitmap_data_;
    }

    bool PngTextureImage::init(const std::string& file_name) {
        png_structp png;
        png_infop info;
        png_byte signature[8];

#ifdef WIN32
        auto file_path_string = std::filesystem::u8path(file_name).wstring();
        FILE* fi = _wfopen(file_path_string.c_str(), L"rb");
#else
        auto file_path_string = std::filesystem::u8path(file_name).u8string();
        FILE* fi = fopen(file_path_string.c_str(), "rb");
#endif
        if (fi == NULL) {
            return false;
        }

        const unsigned int read_size = fread(signature, 1, 8, fi);

        if (png_sig_cmp(signature, 0, 8)) {
            fclose(fi);
            return false;
        }

        png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (png == NULL) {
            fclose(fi);
            return false;
        }

        info = png_create_info_struct(png);
        if (info == NULL) {
            png_destroy_read_struct(&png, NULL, NULL);
            fclose(fi);
            return false;
        }

        png_init_io(png, fi);
        png_set_sig_bytes(png, read_size);
        png_read_png(png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_STRIP_ALPHA, NULL);

        const unsigned int width = png_get_image_width(png, info);
        const unsigned int height = png_get_image_height(png, info);
        const png_byte type = png_get_color_type(png, info);

        const png_bytepp datap = png_get_rows(png, info);

        if (type != PNG_COLOR_TYPE_RGB) {
            std::cerr << "Invalid png type." << std::endl;
            png_destroy_read_struct(&png, &info, NULL);
            fclose(fi);
            return false;
        }

        image_width_ = width;
        image_height_ = height;
        image_channels_ = 3;

        const size_t row_stride = image_width_ * image_channels_;

        bitmap_data_.clear();
        bitmap_data_.reserve(image_height_);

        for (int j = 0; j < image_height_; j++) {

            std::vector<uint8_t> vec(row_stride);
            uint8_t* p = vec.data();

            memcpy(p, datap[j], image_width_ * image_channels_);
            bitmap_data_.push_back(vec);
        }

        png_destroy_read_struct(&png, &info, NULL);
        fclose(fi);

        return true;
    }

    void PngTextureImage::packTo(TextureImageBase* dest, size_t x_delta, size_t y_delta) {
        auto d = dynamic_cast<JpegTextureImage*>(dest);
        if(d == nullptr) {
            std::runtime_error("dest of packTo only supports jpeg.");
        }
        auto srcHeight = getHeight();

        const auto from_vector = getBitmapData().data();
        auto& to = d->getBitmapData();

        for (auto y = 0; y < srcHeight; ++y) {
            std::copy(from_vector[y].begin(), from_vector[y].end(), to.begin() + (y + y_delta) * d->getWidth() * 3 + x_delta * 3);
        }
    }
}
