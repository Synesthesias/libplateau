
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
    // �摜�T�C�Y�ƁA�h��Ԃ��w�i�̃O���[�l�ŁA��̃e�N�X�`���摜���쐬
    TextureImage::TextureImage(size_t w, size_t h, size_t color) {

        textureType = TextureType::Jpeg;
        jpegImage.init(w, h, color);
    }

    // �w�肳�ꂽ�t�@�C������摜��ǂݍ��݁A�e�N�X�`���摜���쐬
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

    // �w�肳�ꂽ�t�@�C�����ŁAjpeg�t�@�C����ۑ�
    void TextureImage::save(const std::string& fileName) const {

        jpegImage.save(fileName);
    }

    // �w�肳�ꂽ���W�ixdelta�Aydelta�j��image�̉摜��]��
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

