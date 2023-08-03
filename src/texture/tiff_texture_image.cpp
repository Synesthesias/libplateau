
#include <plateau/texture/tiff_texture_image.h>

#include <tiffio.h>
#include <string>
#include <vector>
#include <filesystem>

namespace plateau::texture {
    TiffTextureImage::TiffTextureImage() : image_width(0), image_height(0), image_channels(0) {

    }

    std::vector<std::vector<uint8_t>>&
        TiffTextureImage::getBitmapData() {
        return bitmapData;
    }

    //// 指定されたファイルから画像を読み込み、テクスチャ画像を作成
    bool
        TiffTextureImage::init(const std::string& fileName) {
        TIFF* tiff;
        unsigned int bits_per_sample = 0;

        if (fileName.empty()) {
            return false;
        }

#ifdef WIN32
        const auto u8_file_name = std::filesystem::u8path(fileName).wstring();
#else
        const auto u8_file_name = std::filesystem::u8path(fileName).u8string();
#endif

        tiff = TIFFOpenW(u8_file_name.c_str(), "r");
        if (tiff == nullptr) {
            return false;
        }

        if (!TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &image_height)) {
            TIFFClose(tiff);
            return false;
        }

        if (!TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &image_width)) {
            TIFFClose(tiff);
            return false;
        }

        if (!TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bits_per_sample)) {
            TIFFClose(tiff);
            return false;
        }

        if (bits_per_sample != 8) {
            TIFFClose(tiff);
            return false;
        }

        if (!TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &image_channels)) {
            TIFFClose(tiff);
            return false;
        }

        const auto rgba_data = static_cast<unsigned int*>(malloc(sizeof(unsigned int) * image_width * image_height));
        if (rgba_data == nullptr) {
            TIFFClose(tiff);
            return false;
        }

        if (!TIFFReadRGBAImage(tiff, image_width, image_height, rgba_data, 0)) {
            TIFFClose(tiff);
            return false;
        }
        TIFFClose(tiff);

        const size_t row_stride = image_width * image_channels;

        bitmapData.clear();
        bitmapData.reserve(image_height);

        // 画像の上下反転処理でコピー
        for (int j = 0; j < image_height; j++) {

            std::vector<uint8_t> vec(row_stride);
            uint8_t* p = vec.data();

            for (int i = 0; i < image_width; i++) {
                *p++ = TIFFGetR(rgba_data[i + (image_height - 1 - j) * image_width]);
                *p++ = TIFFGetG(rgba_data[i + (image_height - 1 - j) * image_width]);
                *p++ = TIFFGetB(rgba_data[i + (image_height - 1 - j) * image_width]);
            }
            bitmapData.push_back(vec);
        }

        free(rgba_data);

        return true;
    }
} // namespace tiff

