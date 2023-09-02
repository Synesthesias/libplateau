#include <plateau/texture/texture_image_base.h>
#include <plateau/texture/jpeg_texture_image.h>
#include <plateau/texture/png_texture_image.h>
#include <plateau/texture/tiff_texture_image.h>
#include <filesystem>
#include <iostream>

namespace plateau::texture {
    namespace fs = std::filesystem;

    std::unique_ptr<TextureImageBase> TextureImageBase::tryCreateFromFile(const std::string& file_path, const size_t height_limit, bool& out_result) {

        auto extension = fs::u8path(file_path).extension().u8string();

        if (extension == ".jpg" || extension == ".jpeg") {
            auto jpeg_image = std::make_unique<JpegTextureImage>(file_path, height_limit);
            out_result = jpeg_image->loadSucceed();
            return jpeg_image;
        } else if (extension == ".tif" || extension == ".tiff") {
            auto tiff_image = std::make_unique<TiffTextureImage>(file_path);
            out_result = tiff_image->loadSucceed();
            return tiff_image;
        } else if (extension == ".png") {
            auto png_image = std::make_unique<PngTextureImage>(file_path);
            out_result = png_image->loadSucceed();
            return png_image;
        }
        out_result = false;
        return std::make_unique<EmptyTexture>();
    }


    std::unique_ptr<TextureImageBase> TextureImageBase::createNewTexture(size_t width, size_t height) {
        auto jpeg_image = std::make_unique<JpegTextureImage>(width, height, 80);
        return jpeg_image;
    }
}

