#pragma once

#include <memory>
#include <string>
#include <stdexcept>
#include <libplateau_api.h>

namespace plateau::texture {
    /**
     * 画像機能の基底クラスです。
     */
    class LIBPLATEAU_EXPORT TextureImageBase {
    public:

        // 実装上の注意:
        // すべての子クラスがこのクラスの抽象関数をすべて実装しているわけではないことに注意してください。
        // 例えば、save()できるのはJpegTextureImageのみであり、PngTextureImage, TiffTextureImageは未実装につき例外を発します。
        // packTo(dest)のdestもJpegTextureImageのみを受け付け、Png, Tiffの場合は例外を発します。
        // それで良い理由:
        // TexturePackerの実装の仕組みは、新しくJpegTextureImageを作り、そこにjpeg, png, tiffファイルの内容を詰め込んでjpegファイルを出力するものです。
        // そのため、画像ファイルのロードはjpeg, png, tiffの3パターンを実装していますが、packする先と出力機能はjpegのみあれば十分です。

        /**
         * ファイルパスから画像を読み込みます。
         * 拡張子による場合分けで、JpegTextureImage, PngTextureImage, TiffTextureImageのいずれかを返します。
         */
        static std::unique_ptr<TextureImageBase> tryCreateFromFile(const std::string& file_path, size_t height_limit, bool& out_result);
        /**
         * 指定サイズのJpegTextureImageをメモリ上に作ります。
         */
        static std::unique_ptr<TextureImageBase> createNewTexture(size_t width, size_t height);

        virtual size_t getWidth() const = 0;
        virtual size_t getHeight() const = 0;
        virtual bool save(const std::string& file_path) = 0;
        virtual void packTo(TextureImageBase* dest, size_t x_delta, size_t y_delta) = 0;
        virtual const std::string& getFilePath() const = 0;
        virtual bool loadSucceed() const = 0;
        virtual ~TextureImageBase() = default;

    private:
    };

    class EmptyTexture : public TextureImageBase {
    public:
        size_t getWidth() const override {
            return 0;
        };

        size_t getHeight() const override {
            return 0;
        };

        virtual bool save(const std::string& file_path) override{
            throw std::runtime_error("Called save on EmptyTexture.");
        };

        virtual void packTo(TextureImageBase* dest, const size_t x_delta, const size_t y_delta) override{
            throw std::runtime_error("Called packTo on EmptyTexture.");
        }

        virtual const std::string& getFilePath() const override {
            return empty_file_path_;
        }

        virtual bool loadSucceed() const override {
            return false;
        }

        const std::string empty_file_path_ = "";
    };
}
