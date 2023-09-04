
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
        colourSpace = JCS_RGB;

        bitmap_data_ = std::vector(image_height * image_width_ * image_channels_, color);
        assert(getWidth() * getHeight() * image_channels_ == bitmap_data_.size());
    }

    bool JpegTextureImage::init(const std::string& file_name, const size_t height_limit) {

#ifdef WIN32
        const auto regular_name = std::filesystem::u8path(file_name).wstring();
        auto jpeg_file = _wfopen(regular_name.c_str(), L"rb");
#else
        const auto regular_name = std::filesystem::u8path(file_name).u8string();
        auto jpeg_file = fopen(regular_name.c_str(), "rb");
#endif
        if(jpeg_file == nullptr){
            throw std::runtime_error("could not open jpeg file.");
        }

        fseek(jpeg_file, 0, SEEK_END);
        auto jpeg_file_size = ftell(jpeg_file);
        fseek(jpeg_file, 0, SEEK_SET);
        if(jpeg_file_size < 0){
            throw std::runtime_error("could not get file size of jpeg.");
        }
        auto jpeg_buf = (unsigned char*)tj3Alloc(jpeg_file_size);
        fread(jpeg_buf, jpeg_file_size, 1, jpeg_file);
        fclose(jpeg_file);

        auto tj_instance = tj3Init(TJINIT_DECOMPRESS);
        if(tj_instance == nullptr) {
            throw std::runtime_error("could not create jpeg decompress instance");
        }

        tj3DecompressHeader(tj_instance, jpeg_buf, jpeg_file_size);
        auto w = tj3Get(tj_instance, TJPARAM_JPEGWIDTH);
        if( w < 0 ) {
            throw std::runtime_error("failed to get jpeg width.");
        }
        filePath = file_name;
        image_width_ = w;
        image_height = tj3Get(tj_instance, TJPARAM_JPEGHEIGHT);
        auto in_subsamp = tj3Get(tj_instance, TJPARAM_SUBSAMP);
        colourSpace = tj3Get(tj_instance, TJPARAM_COLORSPACE);
        auto pixel_format = TJPF_RGB;
        image_channels_ = tjPixelSize[pixel_format];
        bitmap_data_ = std::vector<unsigned char>(image_width_ * image_height * image_channels_);
        tj3Decompress8(tj_instance, jpeg_buf, jpeg_file_size, bitmap_data_.data(), 0, pixel_format);
        tj3Free(jpeg_buf); jpeg_buf = nullptr;
        tj3Destroy(tj_instance); tj_instance = nullptr;
        return true;


//        try {
//            auto decomp = [](jpeg_decompress_struct* decomp) {
//                jpeg_destroy_decompress(decomp);
//                delete(decomp);
//            };
//
//            std::unique_ptr<jpeg_decompress_struct, decltype(decomp)> decompressInfo(new jpeg_decompress_struct, decomp);
//            jpegErrorManager = std::make_shared<jpeg_error_mgr>();
//
//            auto ptr = [](FILE* fp) {
//                fclose(fp);
//            };
//
//#ifdef WIN32
//            const auto regular_name = std::filesystem::u8path(file_name).wstring();
//            std::unique_ptr<FILE, decltype(ptr)> inFile(_wfopen(regular_name.c_str(), L"rb"), ptr);
//#else
//            const auto regular_name = std::filesystem::u8path(file_name).u8string();
//            std::unique_ptr<FILE, decltype(ptr)> inFile(fopen(regular_name.c_str(), "rb"), ptr);
//#endif
//            if (inFile == nullptr) {
//                throw std::runtime_error("ERROR: Open: " + file_name);
//            }
//
//            decompressInfo->err = jpeg_std_error(jpegErrorManager.get());
//            jpegErrorManager->error_exit = [](j_common_ptr cinfo) {
//                char jpegLastErrorMsg[JMSG_LENGTH_MAX];
//                (*(cinfo->err->format_message))(cinfo, jpegLastErrorMsg);
//                throw std::runtime_error(jpegLastErrorMsg);
//            };
//
//            jpeg_create_decompress(decompressInfo.get());
//
//            jpeg_stdio_src(decompressInfo.get(), inFile.get());
//
//            if (jpeg_read_header(decompressInfo.get(), TRUE) != 1) {
//                throw std::runtime_error("ERROR: Header: ");
//            }
//
//            filePath = file_name;
//            image_width_ = decompressInfo->image_width;
//            image_height = decompressInfo->image_height;
//            image_channels_ = decompressInfo->num_components;
//            colourSpace = decompressInfo->jpeg_color_space;
//
//            if (image_height > height_limit) {
//                jpeg_abort_decompress(decompressInfo.get());
//                return false;
//            }
//
//            jpeg_start_decompress(decompressInfo.get());
//
//            const size_t row_stride = image_width_ * image_channels_;
//            bitmap_data_ = std::vector<uint8_t>(row_stride * image_height);
//            uint8_t* p = bitmap_data_.data();
//            while (decompressInfo->output_scanline < image_height) {
//                jpeg_read_scanlines(decompressInfo.get(), &p, 1);
//                p += row_stride;
//            }
//            assert(p == (&bitmap_data_.at(bitmap_data_.size() - 1)) + 1);
//            jpeg_finish_decompress(decompressInfo.get());
//            assert(bitmap_data_.size() == image_width_ * image_height * image_channels_);
//    }
//        catch (...) {
//            std::cout << "jpeg load error" << std::endl;
//            return false;
//        }
//        return true;
}

    // 指定されたファイル名で、jpegファイルを保存
    bool JpegTextureImage::save(const std::string& file_path) {
        try {
#ifdef WIN32
            const auto regular_name = std::filesystem::u8path(file_path).wstring();
            FILE* outFile = _wfopen(regular_name.c_str(), L"wb");
#else
            const auto regular_name = std::filesystem::u8path(file_path).u8string();
            FILE* outFile = fopen(regular_name.c_str(), "wb");
#endif
            if (outFile == nullptr) {
                return false;
        }

            auto decomp = [](jpeg_compress_struct* comp) {
                jpeg_destroy_compress(comp);
                delete(comp);
            };

            std::unique_ptr<::jpeg_compress_struct, decltype(decomp)> compInfo(new jpeg_compress_struct, decomp);
            jpeg_create_compress(compInfo.get());
            jpeg_stdio_dest(compInfo.get(), outFile);
            compInfo->image_width = image_width_;
            compInfo->image_height = image_height;
            compInfo->input_components = image_channels_;
            compInfo->in_color_space = JCS_RGB;//static_cast<::J_COLOR_SPACE>(colourSpace);
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
            assert(read_ptr == bitmap_data_.data() + bitmap_data_.size());
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

    void JpegTextureImage::packTo(TextureImageBase* dest, const size_t x_delta, const size_t y_delta) {
        auto d = dynamic_cast<JpegTextureImage*>(dest);
        if(d == nullptr) {
            throw std::runtime_error("dest of packTo only supports jpeg.");
        }
        const auto src_height = getHeight();
        const auto src_stride = getWidth() * image_channels_;
        const auto dst_stride = d->getWidth() * d->image_channels_;

        const auto& src = bitmap_data_;
        auto& dst = d->bitmap_data_;

        for (size_t y = 0; y < src_height; ++y) {
            std::copy(src.begin() + y * src_stride, src.begin() + (y + 1) * src_stride,
                      dst.begin() + (y + y_delta) * dst_stride + x_delta * d->image_channels_);
        }
    }

} // namespace jpeg

