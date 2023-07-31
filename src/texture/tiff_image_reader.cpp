
#include <plateau/texture/tiff_image_reader.h>

#include <tiffio.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <regex>
#include <filesystem>

namespace tif
{
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
        unsigned int bitpersample = 0;
        unsigned int* rgbaData = NULL;
        unsigned char* rgbData = NULL;
        int i, j;

        if (fileName.empty()) {
            return -1;
        }

        tiff = TIFFOpen(fileName.c_str(), "r");
        if (tiff == NULL) {
            return -1;
        }

        if (!TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &image_height)) {
            TIFFClose(tiff);
            return -1;
        }

        if (!TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &image_width)) {
            TIFFClose(tiff);
            return -1;
        }

        if (!TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bitpersample)) {
            TIFFClose(tiff);
            return -1;
        }

        if (bitpersample != 8) {
            TIFFClose(tiff);
            return -1;
        }

        if (!TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &image_channels)) {
            TIFFClose(tiff);
            return -1;
        }

        rgbaData = (unsigned int*)malloc(sizeof(unsigned int) * image_width * image_height);
        if (rgbaData == NULL) {
            TIFFClose(tiff);
            return -1;
        }

        if (!TIFFReadRGBAImage(tiff, image_width, image_height, rgbaData, 0)) {
            free(rgbData);
            TIFFClose(tiff);
            return -1;
        }
        TIFFClose(tiff);

        size_t row_stride = image_width * image_channels;

        bitmapData.clear();
        bitmapData.reserve(image_height);

        // 画像の上下反転処理でコピー
        for (j = 0; j < image_height; j++) {

            std::vector<uint8_t> vec(row_stride);
            uint8_t* p = vec.data();

            for (i = 0; i < image_width; i++) {
                *p++ = TIFFGetR(rgbaData[i + (image_height - 1 - j) * image_width]);
                *p++ = TIFFGetG(rgbaData[i + (image_height - 1 - j) * image_width]);
                *p++ = TIFFGetB(rgbaData[i + (image_height - 1 - j) * image_width]);
            }
            bitmapData.push_back(vec);
        }
        free(rgbaData);
        return 0;
    }
} // namespace tiff

