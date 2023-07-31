
#include <plateau/texture/image_reader.h>

#include <jpeglib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace image_reader
{
    // 画像サイズと、塗りつぶす背景のグレー値で、空のテクスチャ画像を作成
    TextureImage::TextureImage(size_t w, size_t h, size_t color) {

        textureType = TextureType::Jpeg;
        jpegImage.init(w, h, color);
    }

    // 指定されたファイルから画像を読み込み、テクスチャ画像を作成
    TextureImage::TextureImage(const std::string& fileName) {

        reset();

        imageFilePath = fileName;

        if (checkFileExtension(fileName, jpegExtension)) {
            textureType = TextureType::Jpeg;
            jpegImage.init(fileName);

            imageWidth = jpegImage.getWidth();
            imageHeight = jpegImage.getHeight();
        }
        else if (checkFileExtension(fileName, tiffExtension)) {
            textureType = TextureType::Tiff;
            tiffImage.init(fileName);

            imageWidth = tiffImage.getWidth();
            imageHeight = tiffImage.getHeight();
        }
        else if (checkFileExtension(fileName, pngExtension)) {
            textureType = TextureType::Png;
            pngImage.init(fileName);

            imageWidth = pngImage.getWidth();
            imageHeight = pngImage.getHeight();
        }
    }

    void
    TextureImage::init(size_t width, size_t height, unsigned char gray) {
        imageWidth = width;
        imageHeight = height;
        imageColor = gray;
        jpegImage.init(width, height, gray);
    }

    void
        TextureImage::reset() {
        imageWidth = 0;
        imageHeight = 0;
        imageColor = 0;
        textureType = TextureType::None;
    }

    bool
    TextureImage::TextureImage::checkFileExtension(const std::string& fileName, const std::string& fileExtension) {
        auto isValid = false;

        size_t lastPos = fileName.find(fileExtension);
        if (fileName.length() == (lastPos + fileExtension.length())) {
            isValid = true;
        }
        return isValid;
    }

    // 指定されたファイル名で、jpegファイルを保存
    void TextureImage::save(const std::string& fileName) const {

        jpegImage.save(fileName);
    }

    // 指定された座標（xdelta、ydelta）にimageの画像を転送
    void
    TextureImage::pack(size_t xdelta, size_t ydelta, TextureImage& image) {

        JpegTextureImage jimg = jpegImage;
        if (image.textureType == TextureType::Jpeg) {
            auto pimage = image.jpegImage;
            pack(xdelta, ydelta, pimage, jpegImage);
        }
        else if (image.textureType == TextureType::Tiff) {
            auto pimage = image.tiffImage;
            pack(xdelta, ydelta, pimage, jpegImage);
        }
        else if (image.textureType == TextureType::Png) {
            auto pimage = image.pngImage;
            pack(xdelta, ydelta, pimage, jpegImage);
        }
    }

    // 指定された座標（xdelta、ydelta）にimageの画像を転送
    void
    TextureImage::pack(size_t xdelta, size_t ydelta, JpegTextureImage& image, JpegTextureImage& targetImage) {
        auto srcHeight = image.getHeight();

        auto fromPtr = image.getBitmapData().data();
        auto toPtr = targetImage.getBitmapData().data();

        for (auto y = 0; y < srcHeight; ++y) {
            std::copy(fromPtr[y].begin(), fromPtr[y].end(), toPtr[ydelta + y].begin() + xdelta * 3);
        }
    }

    void
        TextureImage::pack(size_t xdelta, size_t ydelta, TiffTextureImage& image, JpegTextureImage& targetImage) {
        auto srcHeight = image.getHeight();

        auto fromPtr = image.getBitmapData().data();
        auto toPtr = targetImage.getBitmapData().data();

        for (auto y = 0; y < srcHeight; ++y) {
            std::copy(fromPtr[y].begin(), fromPtr[y].end(), toPtr[ydelta + y].begin() + xdelta * 3);
        }
    }

    void
    TextureImage::pack(size_t xdelta, size_t ydelta, PngTextureImage& image, JpegTextureImage& targetImage) {
        auto srcHeight = image.getHeight();

        auto fromPtr = image.getBitmapData().data();
        auto toPtr = targetImage.getBitmapData().data();

        for (auto y = 0; y < srcHeight; ++y) {
            std::copy(fromPtr[y].begin(), fromPtr[y].end(), toPtr[ydelta + y].begin() + xdelta * 3);
        }
    }
} // namespace image_reader

