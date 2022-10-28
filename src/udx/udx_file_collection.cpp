#include <filesystem>
#include <utility>

#include <plateau/udx/udx_file_collection.h>
#include <fstream>
#include <queue>
#include <set>
#include <regex>
#include "plateau/geometry/geo_reference.h"

namespace plateau::udx {
    namespace fs = std::filesystem;

    PredefinedCityModelPackage UdxSubFolder::getPackage(const std::string& folder_name) {
        if (folder_name == bldg) return PredefinedCityModelPackage::Building;
        if (folder_name == tran) return PredefinedCityModelPackage::Road;
        if (folder_name == urf) return PredefinedCityModelPackage::UrbanPlanningDecision;
        if (folder_name == luse) return PredefinedCityModelPackage::LandUse;
        if (folder_name == frn) return PredefinedCityModelPackage::CityFurniture;
        if (folder_name == veg) return PredefinedCityModelPackage::Vegetation;
        if (folder_name == dem) return PredefinedCityModelPackage::Relief;
        if (folder_name == fld) return PredefinedCityModelPackage::DisasterRisk;
        if (folder_name == tnm) return PredefinedCityModelPackage::DisasterRisk;
        if (folder_name == lsld) return PredefinedCityModelPackage::DisasterRisk;
        if (folder_name == htd) return PredefinedCityModelPackage::DisasterRisk;
        if (folder_name == ifld) return PredefinedCityModelPackage::DisasterRisk;
        return PredefinedCityModelPackage::Unknown;
    }

    CityModelPackageInfo UdxSubFolder::getPackageInfo(const std::string& folder_name) {
        return CityModelPackageInfo::getPredefined(getPackage(folder_name));
    }

    std::shared_ptr<UdxFileCollection> UdxFileCollection::find(const std::string& source) {
        auto result = std::make_shared<UdxFileCollection>();
        find(source, *result);
        return result;
    }

    namespace {
        /**
         * �w��p�X���N�_�ɁA���D��T��(BFS)��GML�t�@�C�����������܂��B
         * �����̍������̂��߁AGML�t�@�C���̔z�u�ꏊ�̐[���͂��ׂē����ł���Ƃ����O��ɗ����A
         * �ŏ���GML�t�@�C�������������n�_�ł���ȏ�[���t�H���_�̒T���͒��~���܂��B
         * �����[���ɂ���ʂ̃t�H���_�͒T���ΏۂƂ��܂��B
         * @param dir_path  �����̋N�_�ƂȂ�p�X�ł��B
         * @param result ���ʂ͂��� vector �ɒǉ�����܂��B
         * @return GML�t�@�C���̃p�X�� vector �ł��B
         */
        void findGMLsBFS(const std::string& dir_path, std::vector<GmlFileInfo>& result) {
            auto queue = std::queue<std::string>();
            queue.push(dir_path);
            bool push_more_dir = true;
            while (!queue.empty()) {
                auto next_dir = queue.front();
                queue.pop();
                // �t�@�C�����猟�����܂��B
                for (const auto& entry : fs::directory_iterator(next_dir)) {
                    if (entry.is_directory()) continue;
                    const auto& path = entry.path();
                    if (path.extension() == ".gml") {
                        result.emplace_back(path.string());
                        // �ŏ���GML�t�@�C��������������A����ȏ�T���L���[�ɓ���Ȃ��悤�ɂ��܂��B
                        // �����[���ɂ���t�H���_�͂��łɃL���[�ɓ����Ă���̂ŁA�u�[���͓��������ǃt�H���_���Ⴄ�v�Ƃ����󋵂͌����ΏۂɊ܂܂�܂��B
                        push_more_dir = false;
                    }
                }
                // ���ɒT�����ׂ��f�B���N�g�����L���[�ɓ���܂��B
                if (!push_more_dir) continue;
                for (const auto& entry : fs::directory_iterator(next_dir)) {
                    if (!entry.is_directory()) continue;
                    queue.push(entry.path().string());
                }
            }
        }
    }

    void UdxFileCollection::find(const std::string& source, UdxFileCollection& collection) {
        collection.udx_path_ = fs::u8path(source).append(u"udx").make_preferred().u8string();
        // udx�t�H���_���̊e�t�H���_�ɂ���
        for (const auto& entry : fs::directory_iterator(collection.udx_path_)) {
            if (!entry.is_directory()) continue;
            auto udx_sub_folder = UdxSubFolder(entry.path().filename().string());
            const auto package = udx_sub_folder.getPackage(udx_sub_folder.getName());
            auto& file_map = collection.files_;
            if (file_map.count(package) == 0) {
                file_map.emplace(package, std::vector<GmlFileInfo>());
            }
            auto& gml_files = collection.files_.at(package);
            findGMLsBFS(entry.path().string(), gml_files);
        }
    }

    std::shared_ptr<UdxFileCollection> UdxFileCollection::filter(geometry::Extent extent) {
        auto result = std::make_shared<UdxFileCollection>();
        filter(extent, *result);
        return result;
    }

    void UdxFileCollection::filter(geometry::Extent extent, UdxFileCollection& collection) {
        std::vector<MeshCode> mesh_codes;
        MeshCode::getThirdMeshes(extent, mesh_codes);

        for (const auto& [sub_folder, files] : files_) {
            for (const auto& file : files) {
                for (const auto& mesh_code : mesh_codes) {
                    if (mesh_code.isWithin(file.getMeshCode())) {
                        collection.addFile(sub_folder, file);
                    }
                }
            }
        }
    }

    void UdxFileCollection::filterByMeshCodes(const std::vector<MeshCode>& mesh_codes,
                                              UdxFileCollection& collection) const {
        // ���ꂪ�Ȃ��ƃt�B���^�[�̌��ʂɑ΂��� fetch �����s����Ƃ��Ƀp�X������܂��B
        collection.setUdxPath(udx_path_);
        // �����p�ɁA������ mesh_codes �𕶎���̃Z�b�g�ɂ��܂��B
        auto mesh_codes_str_set = std::set<std::string>();
        for (const auto& mesh_code : mesh_codes) {
            mesh_codes_str_set.insert(mesh_code.get());
        }
        // �t�@�C�����Ƃ� mesh_codes_str_set �Ɋ܂܂��Ȃ�ǉ����Ă����܂��B
        for (const auto& [sub_folder, files] : files_) {
            for (const auto& file : files) {
                if (mesh_codes_str_set.find(file.getMeshCode().get()) != mesh_codes_str_set.end()) {
                    collection.addFile(sub_folder, file);
                }
            }
        }
    }

    std::shared_ptr<UdxFileCollection>
        UdxFileCollection::filterByMeshCodes(const std::vector<MeshCode>& mesh_codes) const {
        auto result = std::make_shared<UdxFileCollection>();
        filterByMeshCodes(mesh_codes, *result);
        return result;
    }

    PredefinedCityModelPackage UdxFileCollection::getPackages() {
        auto result = PredefinedCityModelPackage::None;
        for (const auto& [key, _] : files_) {
            result = result | key;
        }
        return result;
    }

    const std::string& UdxFileCollection::getGmlFilePath(PredefinedCityModelPackage package, int index) {
        return getGmlFileInfo(package, index).getPath();
    }

    const GmlFileInfo& UdxFileCollection::getGmlFileInfo(PredefinedCityModelPackage package, int index) {
        if (files_.find(package) == files_.end())
            throw std::out_of_range("Key not found");

        if (index >= files_[package].size())
            throw std::out_of_range("Index out of range");

        return files_[package][index];
    }

    int UdxFileCollection::getGmlFileCount(PredefinedCityModelPackage package) {
        if (files_.find(package) == files_.end())
            throw std::out_of_range("Key not found");

        return (int)files_[package].size();
    }

    std::shared_ptr<std::vector<std::string>> UdxFileCollection::getGmlFiles(PredefinedCityModelPackage package) {
        const auto result = std::make_shared<std::vector<std::string>>();

        if (files_.find(package) == files_.end())
            return result;

        for (const auto file : files_[package]) {
            result->push_back(file.getPath());
        }
        return result;
    }

    // fetch �Ŏg�������֐�
    namespace {
        using ConstStrIterT = decltype(std::string("a").cbegin());

        /**
         * @brief ���K�\���Ō������܂����A�q���g��^���邱�ƂŌ��������������܂��B
         *        �q���g�Ƃ͉����ɂ��ẮA �֐� searchAllStringsBetween �̃R�����g���Q�Ƃ��Ă��������B
         * @param str �����Ώۂ̕�����ł��B
         * @param search_pos �����̊J�n�ʒu�ł��B
         * @param matched �������ʂ͂����Ɋi�[����܂��B
         * @param regex �������鐳�K�\���ł��B
         * @param hint �q���g������ł��B���K�\���������q�b�g����Ƃ��A�q���g�����񂪕K�������q�b�g���A
         *             �����̃q���g������̎��͂̎w��o�C�g���ɂ��̐��K�\���q�b�g�����񂪊܂܂�邱�Ƃ������ƂȂ�܂��B
         * @param search_range_before_hint �q���g������̑O�̉��o�C�g�ڂ��琳�K�\���ɂ�錟�����n�߂邩�ł��B
         * @param search_range_after_hint �q���g������̌�̉��o�C�g�ڂ܂Ő��K�\���ɂ�錟���Ώۂɂ��邩�ł��B
         * @return �q�b�g������true, �Ȃ���� false ��Ԃ��܂��B
         */
        bool regexSearchWithHint(const std::string& str, ConstStrIterT search_pos, std::smatch& matched,
                                 const std::regex& regex, const std::string& hint,
                                 unsigned search_range_before_hint, unsigned search_range_after_hint
        ) {
            const auto str_begin = str.cbegin();
            while (search_pos != str.cend()) {
                // �q���g�Ō������܂��B
                auto hint_matched_pos = str.find(hint, search_pos - str_begin);
                // �q���g�Ō����q�b�g���Ȃ���΁A���K�\���ł������q�b�g���܂���B���̂悤�ȃq���g���n����Ă��邱�Ƃ��O��ł��B
                if (hint_matched_pos == std::string::npos) return false;
                // �q���g�������q�b�g�����̂ŁA���̎��͂̎w�萔�̃o�C�g�͈͂𐳋K�\���̌����͈͂ɂ��܂��B
                auto search_start =
                    str_begin + std::max((long long)0, (long long)hint_matched_pos - search_range_before_hint);
                auto search_end = std::min(str.end(), str_begin + (long long)hint_matched_pos + (long long)hint.size() + search_range_after_hint);
                // ���K�\���Ńq�b�g������A���̌��ʂ����� matched �Ɋi�[���ĕԂ��܂��B
                bool found = std::regex_search(search_start, search_end, matched, regex);
                if (found) return true;
                // �q���g�ɂ̓q�b�g�������ǐ��K�\���Ƀq�b�g���Ȃ������P�[�X�ł��B�����ʒu��i�߂čēx�q���g���������܂��B
                search_pos = std::min(str.cend(), str_begin + (long long)hint_matched_pos + (long long)hint.size());
            }

        }

        /**
         * ���������� str �̂����A���� begin_tag_regex �� end_tag_regex �ň͂܂ꂽ����������ׂČ����� set �ŕԂ��܂��B
         * end_tag_regex �� begin_tag_regex ���o�ꂷ��ӏ����オ�����ΏۂƂȂ�܂��B
         * begin_tag_regex �ɑΉ����� end_tag_regex ���Ȃ��ꍇ�Astr�̖����܂ł��ΏۂƂȂ�܂��B
         * �������ʂ̂��������������1�ɂ܂Ƃ߂��܂��B
         *
         * �q���g�ɂ���:
         * �����̍������̂��߂Ɉ����Ńq���g�������^����K�v������܂��B
         * �Ⴆ��40MB��GML�t�@�C���ɑ΂��ċ𒼂ɐ��K�\���Ō��������1��30�b���x�̎��Ԃ�������܂��B
         * �����Ńq���g��^���邱�ƂŁA���K�\���̌����͈͂����܂� 4�b���x�ɒZ�k�ł��܂����B
         * �q���g������Ƃ́A���K�\���������q�b�g����Ƃ��A���̏ꏊ�Ńq���g��������K���q�b�g����A�Ƃ��������𖞂���������ł��B
         * �Ⴆ�΁A������ <start_tag> ���������������A<����> �̑O��ɔ��p�X�y�[�X�������Ă���P�[�X�������������Ƃ����ꍇ�A
         * �������K�\���� < *start_tag *> �ł���A�q���g������� start_tag �ƂȂ�܂��B
         * ���̂Ƃ��A�܂����K�\���������������Ȓʏ핶���񌟍��� start_tag ����������܂��B
         * �����Č��������ʒu����O��Ɏw��o�C�g���̕����Ƃ����͈͂𐳋K�\���Ō������܂��B
         * ���͈͈̔͂��� search_range_before_hint, search_range_after_hint �Ŏw�肵�܂��B���̒l�͏��Ȃ��ق��������Ȃ�܂��B
         */
        std::set<std::string> searchAllStringsBetween(
                const std::regex& begin_tag_regex, const std::regex& end_tag_regex,
                const std::string& str,
                const std::string& begin_tag_hint, const std::string& end_tag_hint,
                unsigned search_range_before_hint, unsigned search_range_after_hint
        ) {
            std::set<std::string> found;
            std::smatch begin_tag_matched;
            std::smatch end_tag_matched;
            auto begin_tag_search_iter = str.begin();
            while (true) {
                // �J�n�^�O���������܂��B
                if (!regexSearchWithHint(str, begin_tag_search_iter, begin_tag_matched, begin_tag_regex, begin_tag_hint,
                    search_range_before_hint, search_range_after_hint)) {
                    break;
                }
                // �I���^�O���������܂��B
                const auto next_of_begin_tag = begin_tag_matched[0].second;
                if (regexSearchWithHint(str, next_of_begin_tag, end_tag_matched, end_tag_regex, end_tag_hint,
                    search_range_before_hint, search_range_after_hint)) {
                    // �J�n�^�O�ƏI���^�O�ɋ��܂ꂽ����������ʂƂ��Ċi�[���܂��B
                    found.insert(std::string(next_of_begin_tag, end_tag_matched[0].first));
                } else {
                    found.insert(std::string(next_of_begin_tag, str.end()));
                    break;
                }
                const auto next_of_end_tag = end_tag_matched[0].second;
                // �C�e���[�^�[��i�߂܂��B
                begin_tag_search_iter = next_of_end_tag;
            }
            return found;
        }

        std::string loadFile(const fs::path& file_path) {
            std::ifstream ifs(file_path.u8string());
            if (!ifs) {
                throw std::runtime_error(
                        "loadFile : Could not open file " + file_path.u8string());
            }
            std::ostringstream buffer;
            buffer << ifs.rdbuf();
            return buffer.str();
        }

        auto regex_options = std::regex::optimize | std::regex::nosubs;

        std::set<std::string> searchAllImagePathsInGML(const std::string& file_content) {
            // �J�n�^�O�� <app:imageURI> �ł��B�������A<����> �̑O��ɔ��p�X�y�[�X�������Ă��ǂ����̂Ƃ��܂��B
            static const auto begin_tag = std::regex(R"(< *app:imageURI *>)", regex_options);
            // �I���^�O�� </app:imageURI> �ł��B�������A<����> �� /(�X���b�V��) �̑O��ɔ��p�X�y�[�X�������Ă��ǂ����̂Ƃ��܂��B
            static const auto end_tag = std::regex(R"(< */ *app:imageURI *>)", regex_options);
            static auto tag_hint = std::string("app:imageURI");
            auto found_url_strings = searchAllStringsBetween(begin_tag, end_tag, file_content, tag_hint, tag_hint, 5, 10);
            return found_url_strings;
        }

        std::set<std::string> searchAllCodelistPathsInGML(const std::string& file_content) {
            // �J�n�^�O�� codeSpace=" �ł��B������ =(�C�R�[��), "(�_�u���N�H�[�e�[�V����)�̑O��ɔ��p�X�y�[�X�������Ă��ǂ����̂Ƃ��܂��B
            static const auto begin_tag = std::regex(R"(codeSpace *= *")", regex_options);
            // �I���^�O�́A�J�n�^�O�̎��� "(�_�u���N�H�[�e�[�V����)�ł��B
            static const auto end_tag = std::regex(R"(")", regex_options);
            static const auto begin_tag_hint = "codeSpace";
            auto found_strings = searchAllStringsBetween(begin_tag, end_tag, file_content, begin_tag_hint, "\"", 5, 10);
            return found_strings;
        }

        /**
         * ������ set �̒��g�𑊑΃p�X�Ɖ��߂��A set�̊e�v�f���R�s�[���܂��B
         * ���΃p�X�̊�� �R�s�[���� ���� src_base_path�A �R�s�[��� dest_base_path �ɂȂ�܂��B
         * �R�s�[��ɓ����̃t�H���_�����݂���ꍇ�̓R�s�[���܂���B
         * �R�s�[�������݂��Ȃ��ꍇ�̓R�s�[���܂���B
         */
        void copyFiles(const std::set<std::string>& path_set, const fs::path& src_base_path, const fs::path& dest_base_path) {
            for (const auto& path : path_set) {
                auto src = src_base_path;
                auto dest = dest_base_path;
                src.append(path).make_preferred();
                dest.append(path).make_preferred();
                if (!fs::exists(src)) {
                    std::cout << "file not exist : " << src.string() << std::endl;
                    continue;
                }
                fs::create_directories(dest.parent_path());
                fs::copy(src, dest, fs::copy_options::skip_existing);
            }
        }
    } // fetch �Ŏg�������֐�


    std::string UdxFileCollection::fetch(const std::string& destination_root_path, const GmlFileInfo& gml_file) const {
        const auto root_folder_name = fs::u8path(udx_path_).parent_path().filename().string();
        auto destination_root = fs::u8path(destination_root_path);
        const auto destination_udx_path = destination_root.append(root_folder_name).append("udx").string();
        fs::path gml_destination_path(destination_udx_path);
        gml_destination_path.append(getRelativePath(gml_file.getPath()));
        fs::create_directories(gml_destination_path.parent_path());
        const auto& gml_file_path = gml_file.getPath();
        fs::copy(gml_file_path, gml_destination_path, fs::copy_options::skip_existing);

        // GML�t�@�C����ǂݍ��݁A�֘A����e�N�X�`���p�X�ƃR�[�h���X�g�p�X���擾���܂��B
        const auto gml_content = loadFile(gml_file.getPath());
        auto image_paths = searchAllImagePathsInGML(gml_content);
        auto codelist_paths = searchAllCodelistPathsInGML(gml_content);

        for (const auto& path : image_paths) {
            std::cout << path << std::endl;
        }
        for (const auto& path : codelist_paths) {
            std::cout << path << std::endl;
        }

        // �e�N�X�`���ƃR�[�h���X�g�t�@�C�����R�s�[���܂��B
        auto gml_dir_path = fs::path(gml_file_path).parent_path();
        auto app_destination_path = fs::path(destination_udx_path).append(getRelativePath(gml_dir_path.string()));
        copyFiles(image_paths, gml_dir_path, app_destination_path);
        copyFiles(codelist_paths, gml_dir_path, app_destination_path);

        return gml_destination_path.u8string();
    }

    std::string UdxFileCollection::getU8RelativePath(const std::string& path) const {
        return fs::relative(fs::u8path(path), fs::u8path(udx_path_)).u8string();
    }

    TVec3d UdxFileCollection::calculateCenterPoint(const geometry::GeoReference& geo_reference) {
        const auto& mesh_codes = getMeshCodes();
        double lat_sum = 0;
        double lon_sum = 0;
        double height_sum = 0;
        for (const auto& mesh_code : mesh_codes) {
            const auto& center = mesh_code.getExtent().centerPoint();
            lat_sum += center.latitude;
            lon_sum += center.longitude;
            height_sum += center.height;
        }
        auto num = (double)mesh_codes.size();
        geometry::GeoCoordinate geo_average = geometry::GeoCoordinate(lat_sum / num, lon_sum / num, height_sum / num);
        auto euclid_average = geo_reference.project(geo_average);
        return euclid_average;
    }

    std::string UdxFileCollection::getRelativePath(const std::string& path) const {
        return fs::relative(fs::u8path(path).make_preferred(), fs::u8path(udx_path_)).make_preferred().string();
    }

    std::set<MeshCode>& UdxFileCollection::getMeshCodes() {
        if (!mesh_codes_.empty())
            return mesh_codes_;

        for (const auto& [_, files] : files_) {
            for (const auto& file : files) {
                mesh_codes_.insert(file.getMeshCode());
            }
        }
        return mesh_codes_;
    }

    void UdxFileCollection::addFile(PredefinedCityModelPackage sub_folder, const GmlFileInfo& gml_file_info) {
        if (files_.count(sub_folder) <= 0) {
            files_.emplace(sub_folder, std::vector<GmlFileInfo>());
        }
        files_.at(sub_folder).push_back(gml_file_info);
    }

    void UdxFileCollection::setUdxPath(std::string udx_path) {
        udx_path_ = std::move(udx_path);
    }
}