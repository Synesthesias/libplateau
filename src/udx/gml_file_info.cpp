#include <string>
#include <filesystem>

#include <plateau/udx/gml_file_info.h>
#include <plateau/udx/mesh_code.h>

namespace plateau::udx {

    namespace fs = std::filesystem;

    GmlFileInfo::GmlFileInfo(const std::string& path)
        : path_(path),
        is_valid_(false){
        applyPath();
    }

    const std::string& GmlFileInfo::getPath() const {
        return path_;
    }

    void GmlFileInfo::setPath(const std::string& path) {
        path_ = path;
        applyPath();
    }

    MeshCode GmlFileInfo::getMeshCode() const {
        return MeshCode(code_);
    }

    const std::string& GmlFileInfo::getFeatureType() const {
        return feature_type_;
    }

    std::string GmlFileInfo::getAppearanceDirectoryPath() const {
        const auto filename = fs::u8path(path_).filename().u8string();
        auto appearance_path = fs::u8path(path_).parent_path().append("").u8string();
        std::string current;
        int cnt = 0;
        for (const auto& character : filename) {
            appearance_path += character;
            if (character == '_') {
                cnt++;
                if (cnt == 3)
                    break;
            }
        }
        appearance_path += "appearance";
        return appearance_path;
    }

    bool GmlFileInfo::isValid() const {
        return is_valid_;
    }

    void GmlFileInfo::applyPath() {
        const auto filename = fs::u8path(path_).filename().u8string();
        std::vector<std::string> filename_parts;
        std::string current;
        current.reserve(filename.size());
        for (const auto character : filename) {
            if (character == '_') {
                filename_parts.push_back(current);
                current = "";
                continue;
            }
            current += character;
        }
        try {
            code_ = filename_parts.empty() ? "" : filename_parts.at(0);
            feature_type_ = filename_parts.size() <= 1 ? "" : filename_parts.at(1);
            is_valid_ = true;
        }
        catch (...) {
            is_valid_ = false;
        }
    }
}
