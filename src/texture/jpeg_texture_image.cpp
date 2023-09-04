
#include <plateau/texture/jpeg_texture_image.h>

#include <jpeglib.h>
#include <turbojpeg.h>
#include <fstream>
#include <stdexcept>
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

        bitmap_data_ = std::vector(image_height * image_width_ * image_channels_, color);
        assert(getWidth() * getHeight() * image_channels_ == bitmap_data_.size());
    }

    namespace{
        const auto closeFile = [](FILE* f){
            fclose(f);
        };

        const auto deleteJpegBuffer = [](unsigned char* jpeg_buffer) {
            tj3Free(jpeg_buffer);
        };

        const auto deleteTjInstance = [](tjhandle tj_instance) {
            tj3Destroy(tj_instance);
        };
    }

    bool JpegTextureImage::init(const std::string& file_name, const size_t height_limit) {
        // libjpegを使ってjpegを扱うとき、APIは主に2種類あります。jpeg_で始まる名前のAPIか、tj_で始まる名前のAPIです。
        // jpeg_系APIでjpegをロードしたとき、Macでのみ発生する不可解なメモリ系エラーに悩まされました。tj_系APIに移行したところ修正されたという経緯があります。
        try{
#ifdef WIN32
            const auto regular_name = std::filesystem::u8path(file_name).wstring();
            std::unique_ptr<FILE, decltype(closeFile)> jpeg_file_uptr(_wfopen(regular_name.c_str(), L"rb"), closeFile);
#else
            const auto regular_name = std::filesystem::u8path(file_name).u8string();
            std::unique_ptr<FILE, decltype(closeFile)> jpeg_file_uptr(fopen(regular_name.c_str(), "rb"), closeFile);
#endif
            if(jpeg_file_uptr == nullptr){
                throw std::runtime_error("could not open jpeg file.");
            }
            const auto jpeg_file = jpeg_file_uptr.get();

            fseek(jpeg_file, 0, SEEK_END);
            auto jpeg_file_size = ftell(jpeg_file);
            fseek(jpeg_file, 0, SEEK_SET);
            if(jpeg_file_size < 0){
                throw std::runtime_error("could not get file size of jpeg.");
            }
            std::unique_ptr<unsigned char, decltype(deleteJpegBuffer)> jpeg_buf_uptr((unsigned char*)tj3Alloc(jpeg_file_size), deleteJpegBuffer);
            const auto jpeg_buf = jpeg_buf_uptr.get();
            fread(jpeg_buf, jpeg_file_size, 1, jpeg_file);

            std::unique_ptr<void, decltype(deleteTjInstance)> tj_instance_uptr((tjhandle)tj3Init(TJINIT_DECOMPRESS), deleteTjInstance);
            if(tj_instance_uptr == nullptr) {
                throw std::runtime_error("could not create jpeg decompress instance");
            }
            const auto tj_instance = tj_instance_uptr.get();

            tj3DecompressHeader(tj_instance, jpeg_buf, jpeg_file_size);
            auto w = tj3Get(tj_instance, TJPARAM_JPEGWIDTH);
            if( w < 0 ) {
                throw std::runtime_error("failed to get jpeg width.");
            }
            filePath = file_name;
            image_width_ = w;
            image_height = tj3Get(tj_instance, TJPARAM_JPEGHEIGHT);
            auto pixel_format = TJPF_RGB;
            image_channels_ = tjPixelSize[pixel_format];
            bitmap_data_ = std::vector<unsigned char>(image_width_ * image_height * image_channels_);
            tj3Decompress8(tj_instance, jpeg_buf, jpeg_file_size, bitmap_data_.data(), 0, pixel_format);
            return true;
        }catch(...){
            std::cerr << "Failed to load jpeg file." << std::endl;
            return false;
        }
}

    // 指定されたファイル名で、jpegファイルを保存
    bool JpegTextureImage::save(const std::string& file_path) {
        try {
            std::unique_ptr<void, decltype(deleteTjInstance)> tj_instance_uptr((tjhandle)tj3Init(TJINIT_COMPRESS), deleteTjInstance);
            auto tj_instance = tj_instance_uptr.get();
            tj3Set(tj_instance, TJPARAM_SUBSAMP, 0);
            tj3Set(tj_instance, TJPARAM_QUALITY, 90);
            tj3Set(tj_instance, TJPARAM_FASTDCT, 0);
            const auto* const bitmap = bitmap_data_.data();
            auto pixel_format = TJCS_RGB;
            const auto pitch = image_width_ * tjPixelSize[pixel_format];
            size_t jpeg_size = 0;
            unsigned char* jpeg_buf = nullptr;
            tj3Compress8(tj_instance, bitmap, image_width_, pitch, image_height, pixel_format, &jpeg_buf, &jpeg_size);


#ifdef WIN32
            const auto regular_name = std::filesystem::u8path(file_path).wstring();
            std::unique_ptr<FILE, decltype(closeFile)> jpeg_file_uptr(_wfopen(regular_name.c_str(), L"wb"), closeFile);
#else
            const auto regular_name = std::filesystem::u8path(file_path).u8string();
            std::unique_ptr<FILE, decltype(closeFile)> jpeg_file_uptr(fopen(regular_name.c_str(), "wb"), closeFile);
#endif
            if(jpeg_file_uptr == nullptr) {
                throw std::runtime_error("could not open jpeg file for write.");
            }
            fwrite(jpeg_buf, jpeg_size, 1, jpeg_file_uptr.get());
            tj3Free(jpeg_buf); jpeg_buf = nullptr;

    }
        catch (...) {
            std::cerr << "Failed to write jpg file." << std::endl;
            return false;
        }
        return true;
    }

    void JpegTextureImage::packTo(TextureImageBase* dest, const size_t x_delta, const size_t y_delta) {
        const auto src_height = getHeight();
        const auto src_stride = getWidth() * image_channels_;
        const auto dst_stride = dest->getWidth() * 3;

        const auto& src = bitmap_data_;
        auto& dst = dest->getBitmapData();

        for (size_t y = 0; y < src_height; ++y) {
            std::copy(src.begin() + y * src_stride, src.begin() + (y + 1) * src_stride,
                      dst.begin() + (y + y_delta) * dst_stride + x_delta * 3);
        }
    }

} // namespace jpeg

