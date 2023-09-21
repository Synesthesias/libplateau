
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

    PngTextureImage::PngTextureImage(size_t width, size_t height, uint8_t initial_color)
            : image_width_(width), image_height_(height),
            image_channels_(3), load_succeed(true), file_path_() {
        bitmap_data_ = std::vector<uint8_t>(width * height * image_channels_, initial_color);
    }

    std::vector<uint8_t>& PngTextureImage::getBitmapData()  {
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
        png_set_expand(png);
        png_read_png(png, info, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_STRIP_ALPHA | PNG_TRANSFORM_GRAY_TO_RGB, NULL);

        const unsigned int width = png_get_image_width(png, info);
        const unsigned int height = png_get_image_height(png, info);
        const png_byte type = png_get_color_type(png, info);

        if (type != PNG_COLOR_TYPE_RGB && type != PNG_COLOR_TYPE_PALETTE) {
            std::cerr << "Invalid png type." << std::endl;
            png_destroy_read_struct(&png, &info, NULL);
            fclose(fi);
            return false;
        }

        const png_bytepp datap = png_get_rows(png, info);

        image_width_ = width;
        image_height_ = height;
        image_channels_ = 3;

        const size_t row_stride = image_width_ * image_channels_;

        bitmap_data_.clear();
        bitmap_data_.reserve(image_height_ * image_width_ * image_channels_);

        for (int j = 0; j < image_height_; j++) {
            for(int w=0; w < image_width_; w++) {
                for(int c=0; c < image_channels_; c++) {
                    auto png_byte = *(datap[j] + w * image_channels_ + c);
                    bitmap_data_.push_back(png_byte);
                }
            }
        }

        png_destroy_read_struct(&png, &info, NULL);
        fclose(fi);

        return true;
    }

    bool PngTextureImage::save(const std::string& file_path) {
        try {
            // 参考: https://github.com/LuaDist/libpng/blob/master/example.c

#ifdef WIN32
            auto file_path_string = std::filesystem::u8path(file_path).wstring();
        FILE* fp = _wfopen(file_path_string.c_str(), L"wb");
#else
            auto file_path_string = std::filesystem::u8path(file_path).u8string();
            FILE* fp = fopen(file_path_string.c_str(), "wb");
#endif
            if(fp == NULL) {
                throw std::runtime_error("could not open png file to write.");
            }

            png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            if(png_ptr == NULL) {
                fclose(fp);
                throw std::runtime_error("could not create writer of png");
            }

            png_infop info_ptr = png_create_info_struct(png_ptr);
            if(info_ptr == NULL) {
                fclose(fp);
                png_destroy_write_struct(&png_ptr, NULL);
                throw std::runtime_error("could not create png info");
            }

            if(setjmp(png_jmpbuf(png_ptr))) {
                fclose(fp);
                png_destroy_write_struct(&png_ptr, &info_ptr);
                throw std::runtime_error("could not write png file.");
            }

            png_init_io(png_ptr, fp);

            // 参考 : https://gist.github.com/niw/5963798

            // Output is 8bit depth, RGB format.
            png_set_IHDR(
                    png_ptr,
                    info_ptr,
                    image_width_, image_height_,
                    8,
                    PNG_COLOR_TYPE_RGB,
                    PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_DEFAULT,
                    PNG_FILTER_TYPE_DEFAULT
            );
            png_write_info(png_ptr, info_ptr);

            auto row_pointers_vec = std::vector<unsigned char*>(image_height_);
            for(auto i=0; i<image_height_; i++) {
                row_pointers_vec.at(i) = &bitmap_data_.at(i * image_width_ * image_channels_);
            }

            png_write_image(png_ptr, row_pointers_vec.data());
            png_write_end(png_ptr, NULL);

            fclose(fp);
            png_destroy_write_struct(&png_ptr, &info_ptr);
            return true;
        }catch(...){
            std::cerr << "Failed to write png file.";
            return false;
        }

    }

    void PngTextureImage::packTo(TextureImageBase* dest, size_t x_delta, size_t y_delta) {
        auto srcHeight = getHeight();

        auto& to = dest->getBitmapData();
        const auto dest_width = dest->getWidth();

        for (auto y = 0; y < srcHeight; ++y) {
            const auto src_begin = bitmap_data_.begin() + y * image_width_ * image_channels_;
            const auto src_end = bitmap_data_.begin() + (y+1) * image_width_ * image_channels_;
            std::copy(src_begin, src_end, to.begin() + (y + y_delta) * dest_width * 3 + x_delta * 3);
        }
    }
}
