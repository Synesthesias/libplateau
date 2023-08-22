
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
    // �摜�T�C�Y�ƁA�h��Ԃ��w�i�̃O���[�l�ŁA��̃e�N�X�`���摜���쐬
    TextureImage::TextureImage(size_t w, size_t h, size_t color) {

        textureType = TextureType::Jpeg;
        jpegImage.init(w, h, color);

    }

    // �w�肳�ꂽ�t�@�C������摜��ǂݍ��݁A�e�N�X�`���摜���쐬
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

    // �w�肳�ꂽ�t�@�C�����ŁAjpeg�t�@�C����ۑ�
    void TextureImage::save(const std::string& fileName) const {
        //if (textureType == TextureType::Jpeg) {
        //    jpegImage.save(fileName);
        //}
        jpegImage.save(fileName);
    }

    //// �w�肳�ꂽ���W�ixdelta�Aydelta�j��image�̉摜��]��
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

    // �w�肳�ꂽ���W�ixdelta�Aydelta�j��image�̉摜��]��
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

