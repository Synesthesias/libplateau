#include <plateau/texture/texture_image_base.h>
#include <plateau/texture/jpeg_texture_image.h>
#include <plateau/texture/png_texture_image.h>
#include <plateau/texture/tiff_texture_image.h>
#include <filesystem>

namespace plateau::texture {
    namespace fs = std::filesystem;

    std::unique_ptr<TextureImageBase> TextureImageBase::tryCreateFromFile(const std::string& file_path, const size_t height_limit, bool& out_result) {

        auto extension = fs::u8path(file_path).extension().u8string();
        std::unique_ptr<TextureImageBase> tex;
        if (extension == ".jpg" || extension == ".jpeg") {
            tex = std::make_unique<JpegTextureImage>(file_path, height_limit);
        } else if (extension == ".tif" || extension == ".tiff") {
            tex = std::make_unique<TiffTextureImage>(file_path);
        } else if (extension == ".png") {
            tex = std::make_unique<PngTextureImage>(file_path);
        }else {
            tex = std::make_unique<EmptyTexture>();
            out_result = false;
            return tex;
        }
        out_result = tex->loadSucceed();
        return tex;
    }


    std::unique_ptr<TextureImageBase> TextureImageBase::createNewTexture(size_t width, size_t height) {
        auto image = std::make_unique<PngTextureImage>(width, height, 80);
        return image;
    }
}

