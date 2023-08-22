
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct jpeg_error_mgr;

namespace plateau::texture {
    class JpegTextureImage {
    public:
        explicit JpegTextureImage() {
        }

        /**
         * \brief �w�肳�ꂽ�t�@�C������摜��ǂݍ��݁A�e�N�X�`���摜���쐬���܂��B
         * \param file_name �摜�t�@�C���̃p�X
         * \param height_limit �摜�̍��������̒l�𒴂���ꍇ�摜�f�[�^�͓ǂݍ��܂�܂���B
         * \return �ǂݍ��݂ɐ��������ꍇtrue�A����ȊO��false
         */
        bool init(const std::string& file_name, const size_t height_limit);
        void init(size_t width, size_t height, size_t color);
        ~JpegTextureImage() {
        }

        size_t getWidth() const {
            return image_width;
        }
        size_t getHeight() const {
            return image_height;
        }
        std::vector<uint8_t>& getBitmapData() {
            return bitmapData;
        }

        bool save(const std::string& fileName);
        void pack(size_t x_delta, size_t y_delta, const JpegTextureImage& image);

    private:
        std::shared_ptr<::jpeg_error_mgr> jpegErrorManager;
        std::vector<uint8_t> bitmapData;
        std::string                       filePath;
        size_t                            image_width;
        size_t                            image_height;
        size_t                            image_channels;
        int                               colourSpace;
    };

} // namespace plateau::texture
