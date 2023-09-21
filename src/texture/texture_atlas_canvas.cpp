#include <plateau/texture/texture_atlas_canvas.h>
#include <filesystem>
#include <sstream>

namespace plateau::texture {
    namespace fs = std::filesystem;
    void TextureAtlasCanvas::setDefaultSaveFilePathIfEmpty(const std::string& original_file_path) {
        if (!save_file_path_.empty())
            return;

        auto original_path = std::filesystem::u8path(original_file_path);
        const auto original_filename_without_ext = original_path.filename().replace_extension("").u8string();

        for (int cnt = 0;; ++cnt) {
            std::stringstream ss;
            ss << std::setw(6) << std::setfill('0') << cnt;
            std::string num = ss.str();

            const auto new_filename = std::string("packed_image_").append(original_filename_without_ext).append("_").append(num).append(".png");
            const auto& path = original_path.replace_filename(new_filename);
            if (!std::filesystem::is_regular_file(path)) {
                save_file_path_ = path.u8string();
                break;
            }
        }
    }

    void TextureAtlasCanvas::setSaveFilePath(const std::filesystem::path& path){
        save_file_path_ = path.u8string();
    }

    const std::string& TextureAtlasCanvas::getSaveFilePath() const {
        return save_file_path_;
    }

    void TextureAtlasCanvas::flush() {
        bool result = canvas_->save(save_file_path_);
        if(!result) {
            throw std::runtime_error("failed to write image file.");
        }
    }

    bool TextureAtlasCanvas::isTexturePacked(const std::string& src_file_path, AtlasInfo& out_atlas_info) {
        for(const auto& tex_info : packed_textures_info) {
            if(tex_info.getSrcTexturePath() == src_file_path) {
                out_atlas_info = tex_info;
                return true;
            }
        }
        return false;
    }
}
