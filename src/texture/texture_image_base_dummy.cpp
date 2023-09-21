#include <plateau/texture/texture_image_base.h>

// モバイル向けにはビルドが通らない箇所を回避するためのダミーcppです。
namespace plateau::texture {

    std::unique_ptr<TextureImageBase> TextureImageBase::tryCreateFromFile(const std::string& file_path, const size_t height_limit, bool& out_result) {
        throw std::runtime_error("not implemented");
    }


    std::unique_ptr<TextureImageBase> TextureImageBase::createNewTexture(size_t width, size_t height) {
        throw std::runtime_error("not implemented");
    }
}

