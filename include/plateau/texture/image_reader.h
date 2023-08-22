
#pragma once

#include <string>
#include <filesystem>

#include <plateau/texture/jpeg_image_reader.h>
#include <plateau/texture/png_image_reader.h>
#include <plateau/texture/tiff_image_reader.h>

namespace plateau::texture {
    class TextureImage {
    public:
        enum class TextureType {
            None, Jpeg, Png, Tiff
        };

        explicit TextureImage() {
        }

        /**
         * \brief �w�肳�ꂽ�t�@�C������摜��ǂݍ��݁A�e�N�X�`���摜���쐬���܂��B
         * \param file_name �摜�t�@�C���̃p�X
         * \param height_limit �摜�̍��������̒l�𒴂���ꍇ�摜�f�[�^�͓ǂݍ��܂�܂���B
         * \return �ǂݍ��݂ɐ��������ꍇtrue�A����ȊO��false
         */
        explicit TextureImage(const std::string& file_name, const size_t height_limit);
        explicit TextureImage(size_t width, size_t height, size_t gray);
        ~TextureImage() {
        }

        void init(size_t width, size_t height, unsigned char gray);
        void reset();

        size_t getWidth() const {
            return imageWidth;
        }
        size_t getHeight() const {
            return imageHeight;
        }
        TextureType getTextureType() {
            return textureType;
        }
        std::string getImageFilePath() {
            return imageFilePath;
        }
        JpegTextureImage& getJpegImage() {
            return jpegImage;
        }

        void save(const std::string& fileName);
        void pack(size_t xdelta, size_t ydelta, TextureImage& image);
        void pack(size_t xdelta, size_t ydelta, JpegTextureImage& image, JpegTextureImage& targetImage);
        void pack(size_t xdelta, size_t ydelta, TiffTextureImage& image, JpegTextureImage& targetImage);
        void pack(size_t xdelta, size_t ydelta, PngTextureImage& image, JpegTextureImage& targetImage);

    private:
        TextureType textureType;
        JpegTextureImage jpegImage;
        PngTextureImage pngImage;
        TiffTextureImage tiffImage;
        size_t imageWidth;
        size_t imageHeight;
        unsigned char imageColor;

        const std::string pngExtension = ".png";
        const std::string jpegExtension = ".jpg";
        const std::string tiffExtension = ".tif";

        std::string imageFilePath;
        
        bool checkFileExtension(const std::string& fileName, const std::string& fileExtension);
    };
}
