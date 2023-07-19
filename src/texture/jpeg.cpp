
#include <plateau/texture/jpeg.h>

#include <jpeglib.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace plateau::texture {
    // 画像サイズと、塗りつぶす背景のグレー値で、空のテクスチャ画像を作成
    TextureImage::TextureImage(size_t w, size_t h, size_t color) {

        jpegErrorManager = std::make_shared<jpeg_error_mgr>();

        width = w;
        height = h;
        channels = 3;
        colourSpace = JCS_RGB;

        size_t row_stride = width * channels;

        bitmapData.clear();
        bitmapData.reserve(height);

        std::vector<uint8_t> vec(row_stride, 0);

        int index = 0;
        for (int column = 0; column < width; ++column) {
            vec[index++] = color;
            vec[index++] = color;
            vec[index++] = color;
        }

        for (int row = 0; row < height; ++row) {

            ;
            bitmapData.push_back(vec);
        }
    }

    // 指定されたファイルから画像を読み込み、テクスチャ画像を作成
    TextureImage::TextureImage(const std::string& fileName) {

        auto decomp = [](jpeg_decompress_struct* decomp) {
            jpeg_destroy_decompress(decomp);
        };

        std::unique_ptr<jpeg_decompress_struct, decltype(decomp)> decompressInfo(new jpeg_decompress_struct, decomp);
        jpegErrorManager = std::make_shared<jpeg_error_mgr>();

        auto ptr = [](FILE* fp) {
            fclose(fp);
        };

        std::unique_ptr<FILE, decltype(ptr)> inFile(fopen(fileName.c_str(), "rb"), ptr);
        if (inFile.get() == NULL) {
            throw std::runtime_error("ERROR: Open: " + fileName);
        }

        decompressInfo->err = jpeg_std_error(jpegErrorManager.get());
        jpegErrorManager->error_exit = [](j_common_ptr cinfo) {
            char jpegLastErrorMsg[JMSG_LENGTH_MAX];
            (*(cinfo->err->format_message))(cinfo, jpegLastErrorMsg);
            throw std::runtime_error(jpegLastErrorMsg);
        };

        jpeg_create_decompress(decompressInfo.get());

        jpeg_stdio_src(decompressInfo.get(), inFile.get());

        int header = jpeg_read_header(decompressInfo.get(), TRUE);

        if (header != 1) {
            throw std::runtime_error("ERROR: Header: ");
        }

        jpeg_start_decompress(decompressInfo.get());

        filePath = fileName;
        width = decompressInfo->output_width;
        height = decompressInfo->output_height;
        channels = decompressInfo->output_components;
        colourSpace = decompressInfo->out_color_space;

        size_t row_stride = width * channels;

        bitmapData.clear();
        bitmapData.reserve(height);

        while (decompressInfo->output_scanline < height) {
            std::vector<uint8_t> vec(row_stride);
            uint8_t* p = vec.data();
            jpeg_read_scanlines(decompressInfo.get(), &p, 1);
            bitmapData.push_back(vec);
        }
        jpeg_finish_decompress(decompressInfo.get());
    }

    TextureImage::~TextureImage() {
    }

    // 指定されたファイル名で、jpegファイルを保存
    void TextureImage::save(const std::string& fileName) const {
        FILE* outFile = fopen(fileName.c_str(), "wb");
        if (outFile == NULL) {
            throw std::runtime_error("ERROR: Save: " + fileName);
        }

        auto decomp = [](jpeg_compress_struct* comp) {
            jpeg_destroy_compress(comp);
        };

        std::unique_ptr<::jpeg_compress_struct, decltype(decomp)> compInfo(new jpeg_compress_struct, decomp);
        jpeg_create_compress(compInfo.get());
        jpeg_stdio_dest(compInfo.get(), outFile);
        compInfo->image_width = width;
        compInfo->image_height = height;
        compInfo->input_components = channels;
        compInfo->in_color_space = static_cast<::J_COLOR_SPACE>(colourSpace);
        compInfo->err = jpeg_std_error(jpegErrorManager.get());
        jpeg_set_defaults(compInfo.get());
        jpeg_set_quality(compInfo.get(), 90, TRUE);
        jpeg_start_compress(compInfo.get(), TRUE);

        for (auto const& pixelData : bitmapData) {
            JSAMPROW rowData[1];

            rowData[0] = const_cast<::JSAMPROW>(pixelData.data());
            jpeg_write_scanlines(compInfo.get(), rowData, 1);
        }
        jpeg_finish_compress(compInfo.get());
        fclose(outFile);
    }

    // 指定された座標（xdelta、ydelta）にimageの画像を転送
    void TextureImage::pack(size_t xdelta, size_t ydelta, const TextureImage& image) {
        auto srcHeight = image.getHeight();

        auto fromPtr = image.bitmapData.data();
        auto toPtr = bitmapData.data();

        for (auto y = 0; y < srcHeight; ++y) {
            std::copy(fromPtr[y].begin(), fromPtr[y].end(), toPtr[ydelta + y].begin() + xdelta * channels);
        }
    }
} // namespace jpeg

