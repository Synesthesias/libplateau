
#include <plateau/texture/image_reader.h>

#include <jpeglib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>



namespace imageReader
{
    // 画像サイズと、塗りつぶす背景のグレー値で、空のテクスチャ画像を作成
    TextureImage::TextureImage(size_t w, size_t h, size_t color) {

        textureType = TextureType::Jpeg;
        jpegImage.init(w, h, color);

    }

    // 指定されたファイルから画像を読み込み、テクスチャ画像を作成
    TextureImage::TextureImage(const std::string& fileName) {

        std::stringstream ss;

        ss << std::setw(4) << std::setfill('0') << 12; 

        std::string num = ss.str();

        imageFilePath = fileName;

        //DisplayPathInfo(fileName);

        if (checkFileExtension(fileName, jpegExtension)) {
            textureType = TextureType::Jpeg;
            jpegImage.init(fileName);

            imageWidth = jpegImage.getWidth();
            imageHeight = jpegImage.getHeight();
        }
        else if (checkFileExtension(fileName, pngExtension)) {
            textureType = TextureType::Png;
            //pngImage.init(fileName);

            //width = pngImage.getWidth();
            //height = pngImage.getHeight();
        }
    }

    void
    TextureImage::init(size_t width, size_t height, unsigned char gray) {
        imageWidth = width;
        imageHeight = height;
        imageColor = gray;
        jpegImage.init(width, height, gray);
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

    void
    TextureImage::DisplayPathInfo(std::string fileName) {

        path pathToDisplay(fileName);

        auto root_name = pathToDisplay.root_name();
        auto root_path = pathToDisplay.root_path();
        auto relative_path = pathToDisplay.relative_path();
        auto parent_path = pathToDisplay.parent_path();
        auto filename = pathToDisplay.filename();
        auto stem = pathToDisplay.stem();
        auto extension = pathToDisplay.extension();

    }

    // 指定されたファイル名で、jpegファイルを保存
    void TextureImage::save(const std::string& fileName) const {
        //if (textureType == TextureType::Jpeg) {
        //    jpegImage.save(fileName);
        //}
        jpegImage.save(fileName);
    }

    //// 指定された座標（xdelta、ydelta）にimageの画像を転送
    void
    TextureImage::pack(size_t xdelta, size_t ydelta, TextureImage& image) {

        JpegTextureImage jimg = jpegImage;

        //if (image.textureType == TextureType::Png) {
        //    auto pimage = image.pngImage;
        //    pack(xdelta, ydelta, pimage, jpegImage);
        //}
        if (image.textureType == TextureType::Jpeg) {
            auto pimage = image.jpegImage;
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

    //void
    //TextureImage::pack(size_t xdelta, size_t ydelta, PngTextureImage& image, JpegTextureImage& targetImage) {
    //    auto srcHeight = image.getHeight();

    //    auto fromPtr = image.getBitmapData().data();
    //    auto toPtr = targetImage.getBitmapData().data();

    //    for (auto y = 0; y < srcHeight; ++y) {
    //        std::copy(fromPtr[y].begin(), fromPtr[y].end(), toPtr[ydelta + y].begin() + xdelta * 3);
    //    }
    //}
} // namespace jpeg

