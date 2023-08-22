
#include <plateau/texture/jpeg_image_reader.h>

#include <jpeglib.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <regex>
#include <filesystem>

namespace plateau::texture {
    // �摜�T�C�Y�ƁA�h��Ԃ��w�i�̃O���[�l�ŁA��̃e�N�X�`���摜���쐬
    void JpegTextureImage::init(size_t w, size_t h, size_t color) {

        jpegErrorManager = std::make_shared<jpeg_error_mgr>();

        image_width = w;
        image_height = h;
        image_channels = 3;
        colourSpace = JCS_RGB;

        size_t row_stride = image_width * image_channels;


        bitmapData = std::vector(image_height * image_width * image_channels, static_cast<uint8_t>(color));
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

            //auto regularName = std::regex_replace(fileName, std::regex("\"), "/");
            //auto regularName = ReplaceString(fileName, "\\", "/")
#ifdef WIN32
            const auto regular_name = std::filesystem::u8path(file_name).wstring();
            std::unique_ptr<FILE, decltype(ptr)> inFile(_wfopen(regular_name.c_str(), L"rb"), ptr);
#else
            const auto regular_name = std::filesystem::u8path(file_name).u8string();
            std::unique_ptr<FILE, decltype(ptr)> inFile(fopen(regular_name.c_str(), L"rb"), ptr);
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
            image_width = decompressInfo->image_width;
            image_height = decompressInfo->image_height;
            image_channels = decompressInfo->num_components;
            colourSpace = decompressInfo->jpeg_color_space;

            if (image_height > height_limit) {
                jpeg_abort_decompress(decompressInfo.get());
                return false;
            }

            jpeg_start_decompress(decompressInfo.get());

            const size_t row_stride = image_width * image_channels;
            bitmapData = std::vector<uint8_t>(row_stride * image_height);
            uint8_t* p = bitmapData.data();
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

    // �w�肳�ꂽ�t�@�C�����ŁAjpeg�t�@�C����ۑ�
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
            compInfo->image_width = image_width;
            compInfo->image_height = image_height;
            compInfo->input_components = image_channels;
            compInfo->in_color_space = static_cast<::J_COLOR_SPACE>(colourSpace);
            compInfo->err = jpeg_std_error(jpegErrorManager.get());
            jpeg_set_defaults(compInfo.get());
            jpeg_set_quality(compInfo.get(), 90, TRUE);
            jpeg_start_compress(compInfo.get(), TRUE);

            auto read_ptr = bitmapData.data();
            for (int line = 0; line < image_height; ++line) {
                JSAMPROW rowData[1];
                rowData[0] = read_ptr;
                jpeg_write_scanlines(compInfo.get(), rowData, 1);
                read_ptr += image_width * image_channels;
            }
            jpeg_finish_compress(compInfo.get());
            fclose(outFile);
        }
        catch (...) {
            return false;
        }
        return true;
    }

    // �w�肳�ꂽ���W�ixdelta�Aydelta�j��image�̉摜��]��
    void JpegTextureImage::pack(const size_t x_delta, const size_t y_delta, const JpegTextureImage& image) {
        const auto src_height = image.getHeight();
        const auto src_stride = image.getWidth() * image_channels;
        const auto dst_stride = image_width * image_channels;

        const auto& src = image.bitmapData;
        // ReSharper disable once CppLocalVariableMayBeConst
        auto& dst = bitmapData;

        for (size_t y = 0; y < src_height; ++y) {
            std::copy(src.begin() + y * src_stride, src.begin() + (y + 1) * src_stride, dst.begin() + (y + y_delta) * dst_stride + x_delta * image_channels);
        }
    }
} // namespace jpeg

