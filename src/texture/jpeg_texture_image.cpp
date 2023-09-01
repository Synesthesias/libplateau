
#include <plateau/texture/jpeg_texture_image.h>

#include <jpeglib.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <regex>
#include <filesystem>
#include <cassert>
#include <iostream>

#include "plateau/texture/png_texture_image.h"

namespace plateau::texture {
    // 画像サイズと、塗りつぶす背景のグレー値で、空のテクスチャ画像を作成
    void JpegTextureImage::init(size_t w, size_t h, uint8_t color) {

        jpegErrorManager = std::make_shared<jpeg_error_mgr>();

        image_width_ = w;
        image_height = h;
        image_channels_ = 3;
        colourSpace = JCS_RGB;

        size_t row_stride = image_width_ * image_channels_;

        bitmap_data_ = std::vector(image_height * image_width_ * image_channels_, color);
    }

    bool JpegTextureImage::init(const std::string& file_name, const size_t height_limit) {
        try {
            auto decomp = [](jpeg_decompress_struct* decomp) {
                jpeg_destroy_decompress(decomp);
            };

            std::unique_ptr<jpeg_decompress_struct, decltype(decomp)> decompressInfo(new jpeg_decompress_struct, decomp);
            jpegErrorManager = std::make_shared<jpeg_error_mgr>();

            auto ptr = [](FILE* fp) {
                fclose(fp);
            };

#ifdef WIN32
            const auto regular_name = std::filesystem::u8path(file_name).wstring();
            std::unique_ptr<FILE, decltype(ptr)> inFile(_wfopen(regular_name.c_str(), L"rb"), ptr);
#else
            const auto regular_name = std::filesystem::u8path(file_name).u8string();
            std::unique_ptr<FILE, decltype(ptr)> inFile(fopen(regular_name.c_str(), "rb"), ptr);
#endif
            if (inFile.get() == NULL) {
                throw std::runtime_error("ERROR: Open: " + file_name);
        }

            decompressInfo->err = jpeg_std_error(jpegErrorManager.get());
            jpegErrorManager->error_exit = [](j_common_ptr cinfo) {
                char jpegLastErrorMsg[JMSG_LENGTH_MAX];
                (*(cinfo->err->format_message))(cinfo, jpegLastErrorMsg);
                throw std::runtime_error(jpegLastErrorMsg);
            };

            jpeg_create_decompress(decompressInfo.get());

            jpeg_stdio_src(decompressInfo.get(), inFile.get());

            if (jpeg_read_header(decompressInfo.get(), TRUE) != 1) {
                throw std::runtime_error("ERROR: Header: ");
            }

            filePath = file_name;
            image_width_ = decompressInfo->image_width;
            image_height = decompressInfo->image_height;
            image_channels_ = decompressInfo->num_components;
            colourSpace = decompressInfo->jpeg_color_space;

            if (image_height > height_limit) {
                jpeg_abort_decompress(decompressInfo.get());
                return false;
            }

            jpeg_start_decompress(decompressInfo.get());

            const size_t row_stride = image_width_ * image_channels_;
            bitmap_data_ = std::vector<uint8_t>(row_stride * image_height);
            uint8_t* p = bitmap_data_.data();
            while (decompressInfo->output_scanline < image_height) {
                jpeg_read_scanlines(decompressInfo.get(), &p, 1);
                p += row_stride;
            }
            jpeg_finish_decompress(decompressInfo.get());
    }
        catch (...) {
            return false;
        }
        return true;
}

    // 指定されたファイル名で、jpegファイルを保存
    bool JpegTextureImage::save(const std::string& file_name) {
        try {
#ifdef WIN32
            const auto regular_name = std::filesystem::u8path(file_name).wstring();
            FILE* outFile = _wfopen(regular_name.c_str(), L"wb");
#else
            const auto regular_name = std::filesystem::u8path(file_name).u8string();
            FILE* outFile = fopen(regular_name.c_str(), "wb");
#endif
            if (outFile == NULL) {
                return false;
        }

            auto decomp = [](jpeg_compress_struct* comp) {
                jpeg_destroy_compress(comp);
            };

            std::unique_ptr<::jpeg_compress_struct, decltype(decomp)> compInfo(new jpeg_compress_struct, decomp);
            jpeg_create_compress(compInfo.get());
            jpeg_stdio_dest(compInfo.get(), outFile);
            compInfo->image_width = image_width_;
            compInfo->image_height = image_height;
            compInfo->input_components = image_channels_;
            compInfo->in_color_space = static_cast<::J_COLOR_SPACE>(colourSpace);
            compInfo->err = jpeg_std_error(jpegErrorManager.get());
            jpeg_set_defaults(compInfo.get());
            jpeg_set_quality(compInfo.get(), 90, TRUE);
            jpeg_start_compress(compInfo.get(), TRUE);

            auto read_ptr = bitmap_data_.data();
            for (int line = 0; line < image_height; ++line) {
                JSAMPROW rowData[1];
                rowData[0] = read_ptr;
                jpeg_write_scanlines(compInfo.get(), rowData, 1);
                read_ptr += image_width_ * image_channels_;
            }
            jpeg_finish_compress(compInfo.get());
            fclose(outFile);
            assert(image_width_ * image_height * image_channels_ == bitmap_data_.size());
    }
        catch (...) {
            std::cerr << "Failed to write jpg file." << std::endl;
            return false;
        }
        return true;
    }

    // 指定された座標（xdelta、ydelta）にimageの画像を転送
    void JpegTextureImage::pack(const size_t x_delta, const size_t y_delta, const JpegTextureImage& image) {
        const auto src_height = image.getHeight();
        const auto src_stride = image.getWidth() * image_channels_;
        const auto dst_stride = image_width_ * image_channels_;

        const auto& src = image.bitmap_data_;
        // ReSharper disable once CppLocalVariableMayBeConst
        auto& dst = bitmap_data_;

        for (size_t y = 0; y < src_height; ++y) {
            std::copy(src.begin() + y * src_stride, src.begin() + (y + 1) * src_stride, dst.begin() + (y + y_delta) * dst_stride + x_delta * image_channels_);
        }
    }

    void JpegTextureImage::packPng(const size_t x_delta, const size_t y_delta, PngTextureImage& image) {
        const auto src_height = image.getHeight();
        const auto dst_stride = image_width_ * image_channels_;

        const auto& src = image.getBitmapData();
        // ReSharper disable once CppLocalVariableMayBeConst
        auto& dst = bitmap_data_;

        for (auto y = 0; y < src_height; ++y) {
            std::copy(src[y].begin(), src[y].end(), dst.begin() + (y + y_delta) * dst_stride + x_delta * 3);
        }
    }
} // namespace jpeg

