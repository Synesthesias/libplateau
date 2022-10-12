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

    PredefinedCityModelPackage UdxSubFolder::getPackage(const std::string& folder_name){
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

    namespace{
        /**
         * æ?®ãã¹ãèµ·ç¹ã«ãå¹?ªåæ¢ç´¢(BFS)ã§GMLãã¡ã¤ã«ãæ¤ç´¢ãã¾ãã??
         * æ¤ç´¢ã®é«é?åã®ãããGMLãã¡ã¤ã«ã®éç½®å ´æ?ã®æ·±ãã?ãã¹ã¦åãã§ããã¨ã?åæã«ç«ã¡ã?
         * æ?åã?GMLãã¡ã¤ã«ãè¦ã¤ãã£ãå°ç¹ã§ããä»¥ä¸æ·±ã?ã©ã«ã?ã®æ¢ç´¢ã¯ä¸­æ­¢ãã¾ãã??
         * åãæ·±ãã«ããå¥ã®ãã©ã«ã?ã¯æ¢ç´¢å¯¾è±¡ã¨ãã¾ãã??
         * @param dir_path  æ¤ç´¢ã®èµ·ç¹ã¨ãªããã¹ã§ãã??
         * @param result çµæã¯ãã? vector ã«è¿½å?ããã¾ãã??
         * @return GMLãã¡ã¤ã«ã®ãã¹ã® vector ã§ãã??
         */
        void findGMLsBFS(const std::string& dir_path, std::vector<GmlFileInfo>& result){
            auto queue = std::queue<std::string>();
            queue.push(dir_path);
            bool push_more_dir = true;
            while(!queue.empty()){
                auto next_dir = queue.front();
                queue.pop();
                // ãã¡ã¤ã«ããæ¤ç´¢ãã¾ãã??
                for(const auto& entry : fs::directory_iterator(next_dir)){
                    if(entry.is_directory()) continue;
                    const auto& path = entry.path();
                    if(path.extension() == ".gml"){
                        result.emplace_back(path.string());
                        // æ?åã?GMLãã¡ã¤ã«ãè¦ã¤ãã£ãããããä»¥ä¸æ¢ç´¢ã­ã¥ã¼ã«å¥ããªã?ã?«ãã¾ãã??
                        // åãæ·±ãã«ãããã©ã«ã?ã¯ãã§ã«ã­ã¥ã¼ã«å¥ã£ã¦ã?ã®ã§ãã?æ·±ãã?åãã?ãã©ãã©ã«ã?ãéã??ã¨ã?ç¶æ³ã?æ¤ç´¢å¯¾è±¡ã«å«ã¾ãã¾ãã??
                        push_more_dir = false;
                    }
                }
                // æ¬¡ã«æ¢ç´¢ãã¹ããã£ã¬ã¯ããªãã­ã¥ã¼ã«å¥ãã¾ãã??
                if(!push_more_dir) continue;
                for(const auto& entry : fs::directory_iterator(next_dir)){
                    if(!entry.is_directory()) continue;
                    queue.push(entry.path().string());
                }
            }
        }
    }

    void UdxFileCollection::find(const std::string& source, UdxFileCollection& collection) {
        collection.udx_path_ = fs::u8path(source).append(u"udx").make_preferred().u8string();
        // udxãã©ã«ã?å??å?ã©ã«ã?ã«ã¤ã?¦
        for (const auto& entry : fs::directory_iterator(collection.udx_path_)) {
            if(!entry.is_directory()) continue;
            auto udx_sub_folder = UdxSubFolder(entry.path().filename().string());
            const auto package = udx_sub_folder.getPackage(udx_sub_folder.getName());
            auto& file_map = collection.files_;
            if(file_map.count(package) == 0){
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
        // ããããªã?¨ãã£ã«ã¿ã¼ã®çµæã«å¯¾ãã¦ fetch ãå®è¡ããã¨ãã«ãã¹ãããã¾ãã??
        collection.setUdxPath(udx_path_);
        // æ¤ç´¢ç¨ã«ãå¼æ°ã® mesh_codes ãæå­å?ã®ã»ã?ã«ãã¾ãã??
        auto mesh_codes_str_set = std::set<std::string>();
        for (const auto& mesh_code: mesh_codes) {
            mesh_codes_str_set.insert(mesh_code.get());
        }
        // ãã¡ã¤ã«ãã¨ã« mesh_codes_str_set ã«å«ã¾ãããªãè¿½å?ãã¦ã?ã¾ãã??
        for (const auto& [sub_folder, files]: files_) {
            for (const auto& file: files) {
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

    const GmlFileInfo& UdxFileCollection::getGmlFileInfo(PredefinedCityModelPackage package, int index){
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

    // fetch ã§ä½¿ã?¡åé¢æ°
    namespace{
        using ConstStrIterT = decltype(std::string("a").cbegin());

        /**
         * @brief æ­£è¦è¡¨ç¾ã§æ¤ç´¢ãã¾ããããã³ããä¸ãããã¨ã§æ¤ç´¢ãé«é?åãã¾ãã??
         *        ãã³ãã¨ã¯ä½ãã«ã¤ã?¦ã¯ã? é¢æ° searchAllStringsBetween ã®ã³ã¡ã³ããåç?ãã¦ãã ããã?
         * @param str æ¤ç´¢å¯¾è±¡ã®æ?­å?ã§ãã??
         * @param search_pos æ¤ç´¢ã®éå§ä½ç½®ã§ãã??
         * @param matched æ¤ç´¢çµæã¯ããã«æ ¼ç´ããã¾ãã??
         * @param regex æ¤ç´¢ããæ­£è¦è¡¨ç¾ã§ãã??
         * @param hint ãã³ãæå­å?ã§ãã?æ­£è¦è¡¨ç¾ãæ¤ç´¢ãããããã¨ãã?ãã³ãæå­å?ãå¿?æ¤ç´¢ããããã?
         *             ãã¤ãã?ãã³ãæå­å?ã®å¨å²ã®æ?®ãã¤ãæ°ã«ãã?æ­£è¦è¡¨ç¾ãããæå­å?ãå«ã¾ãããã¨ãæ¡ä»¶ã¨ãªãã¾ãã??
         * @param search_range_before_hint ãã³ãæå­å?ã®åã?ä½ãã¤ãç®ããæ­£è¦è¡¨ç¾ã«ããæ¤ç´¢ãå§ãããã§ãã??
         * @param search_range_after_hint ãã³ãæå­å?ã®å¾ã?ä½ãã¤ãç®ã¾ã§æ­£è¦è¡¨ç¾ã«ããæ¤ç´¢å¯¾è±¡ã«ãããã§ãã??
         * @return ããããããtrue, ãªããã° false ãè¿ãã¾ãã??
         */
        bool regexSearchWithHint(const std::string& str, ConstStrIterT search_pos, std::smatch& matched,
                                 const std::regex& regex, const std::string& hint,
                                 unsigned search_range_before_hint, unsigned search_range_after_hint
        ) {
            const auto str_begin = str.cbegin();
            while(search_pos != str.cend()){
                // ãã³ãã§æ¤ç´¢ãã¾ãã??
                auto hint_matched_pos = str.find(hint, search_pos - str_begin);
                // ãã³ãã§æ¤ç´¢ãããããªããã°ãæ­£è¦è¡¨ç¾ã§ãæ¤ç´¢ããããã¾ããããã®ãããªãã³ããæ¸¡ããã¦ã?ãã¨ãåæã§ãã??
                if (hint_matched_pos == std::string::npos) return false;
                // ãã³ããæ¤ç´¢ãããããã?ã§ããã®å¨å²ã®æ?®æ°ã®ãã¤ãç¯?²ãæ­£è¦è¡¨ç¾ã®æ¤ç´¢ç¯?²ã«ãã¾ãã??
                auto search_start =
                        str_begin + std::max((long long) 0, (long long) hint_matched_pos - search_range_before_hint);
                auto search_end = std::min(str.end(), str_begin + (long long) hint_matched_pos + (long long)hint.size() + search_range_after_hint);
                // æ­£è¦è¡¨ç¾ã§ããããããããã®çµæãå¼æ° matched ã«æ ¼ç´ãã¦è¿ãã¾ãã??
                bool found = std::regex_search(search_start, search_end, matched, regex);
                if(found) return true;
                // ãã³ãã«ã¯ããããããã©æ­£è¦è¡¨ç¾ã«ãããããªãã£ãã±ã¼ã¹ã§ãã?æ¤ç´¢ä½ç½®ãé?²ãã¦ååº¦ãã³ããæ¤ç´¢ãã¾ãã??
                search_pos = std::min(str.cend(), str_begin + (long long)hint_matched_pos + (long long)hint.size());
            }

        }

        /**
         * å¼æ°æ?­å? str ã®ã?¡ãå¼æ° begin_tag_regex ã¨ end_tag_regex ã§å²ã¾ããæ?­å?ããã¹ã¦æ¤ç´¢ã? set ã§è¿ãã¾ãã??
         * end_tag_regex ã¯ begin_tag_regex ãç»å ´ããç®?ããå¾ãæ¤ç´¢å¯¾è±¡ã¨ãªãã¾ãã??
         * begin_tag_regex ã«å¯¾å¿ãã? end_tag_regex ããªã??´åã?strã®æ«å°¾ã¾ã§ãå¯¾è±¡ã¨ãªãã¾ãã??
         * æ¤ç´¢çµæã®ã?¡åãæ?­å?ã¯1ã¤ã«ã¾ã¨ãããã¾ãã??
         *
         * ãã³ãã«ã¤ã?¦:
         * æ¤ç´¢ã®é«é?åã®ããã«å¼æ°ã§ãã³ãæå­å?ãä¸ããå¿?¦ãããã¾ãã??
         * ä¾ãã°40MBã®GMLãã¡ã¤ã«ã«å¯¾ãã¦æç´ã«æ­£è¦è¡¨ç¾ã§æ¤ç´¢ããã¨1å?30ç§ç¨åº¦ã®æéããããã¾ãã??
         * å¼æ°ã§ãã³ããä¸ãããã¨ã§ãæ­£è¦è¡¨ç¾ã®æ¤ç´¢ç¯?²ãç­ã¾ã? 4ç§ç¨åº¦ã«ç­ç¸®ã§ãã¾ããã?
         * ãã³ãæå­å?ã¨ã¯ãæ­£è¦è¡¨ç¾ãæ¤ç´¢ãããããã¨ãã?ãã®å ´æ?ã§ãã³ãæå­å?ãå¿?ãããããã?ã¨ã?æ¡ä»¶ãæº?ããæ?­å?ã§ãã??
         * ä¾ãã°ãæå­å? <start_tag> ãæ¤ç´¢ããã?ã?<æ¬å¼§> ã®åå¾ã«åè§ã¹ãã?ã¹ãå?ã£ã¦ã?ã±ã¼ã¹ãæ¤ç´¢ããã?¨ã?å ´åã??
         * æ¤ç´¢æ­£è¦è¡¨ç¾ã¯ < *start_tag *> ã§ããããã³ãæå­å?ã¯ start_tag ã¨ãªãã¾ãã??
         * ãã?ã¨ãã?ã¾ãæ­£è¦è¡¨ç¾æ¤ç´¢ãããé«é?ãªéå¸¸æ?­å?æ¤ç´¢ã§ start_tag ãæ¤ç´¢ããã¾ãã??
         * ããã¦è¦ã¤ãã£ãä½ç½®ããåå¾ã«æ?®ãã¤ãæ°ã®å¹?ã¨ã£ãç¯?²ãæ­£è¦è¡¨ç¾ã§æ¤ç´¢ãã¾ãã??
         * ãã?ç¯?²ã¯å¼æ° search_range_before_hint, search_range_after_hint ã§æ?®ãã¾ãã?ãã®å¤ã¯å°ãªã?»ã?éããªãã¾ãã??
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
                // éå§ã¿ã°ãæ¤ç´¢ãã¾ãã??
                if (!regexSearchWithHint(str, begin_tag_search_iter, begin_tag_matched, begin_tag_regex, begin_tag_hint,
                                         search_range_before_hint, search_range_after_hint)) {
                    break;
                }
                // çµäº?¿ã°ãæ¤ç´¢ãã¾ãã??
                const auto next_of_begin_tag = begin_tag_matched[0].second;
                if (regexSearchWithHint(str, next_of_begin_tag, end_tag_matched, end_tag_regex, end_tag_hint,
                                        search_range_before_hint, search_range_after_hint)) {
                    // éå§ã¿ã°ã¨çµäº?¿ã°ã«æã¾ããæ?­å?ãçµæã¨ãã¦æ ¼ç´ãã¾ãã??
                    found.insert(std::string(next_of_begin_tag, end_tag_matched[0].first));
                } else {
                    found.insert(std::string(next_of_begin_tag, str.end()));
                    break;
                }
                const auto next_of_end_tag = end_tag_matched[0].second;
                // ã¤ã?¬ã¼ã¿ã¼ãé?²ãã¾ãã??
                begin_tag_search_iter = next_of_end_tag;
            }
            return found;
        }

        std::string loadFile(const fs::path& file_path){
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

        std::set<std::string> searchAllImagePathsInGML(const std::string& file_content){
            // éå§ã¿ã°ã¯ <app:imageURI> ã§ãã?ãã?ãã??<æ¬å¼§> ã®åå¾ã«åè§ã¹ãã?ã¹ããã£ã¦ãè¯ã?ã®ã¨ãã¾ãã??
            static const auto begin_tag = std::regex(R"(< *app:imageURI *>)", regex_options);
            // çµäº?¿ã°ã¯ </app:imageURI> ã§ãã?ãã?ãã??<æ¬å¼§> ã¨ /(ã¹ã©ã?·ã¥) ã®åå¾ã«åè§ã¹ãã?ã¹ããã£ã¦ãè¯ã?ã®ã¨ãã¾ãã??
            static const auto end_tag = std::regex(R"(< */ *app:imageURI *>)", regex_options);
            static auto tag_hint = std::string("app:imageURI");
            auto found_url_strings = searchAllStringsBetween(begin_tag, end_tag, file_content, tag_hint, tag_hint, 5, 10);
            return found_url_strings;
        }

        std::set<std::string> searchAllCodelistPathsInGML(const std::string& file_content){
            // éå§ã¿ã°ã¯ codeSpace=" ã§ãã?ãã?ã? =(ã¤ã³ã¼ã«), "(ã?ãã«ã¯ã©ã¼ã??ã·ã§ã³)ã®åå¾ã«åè§ã¹ãã?ã¹ããã£ã¦ãè¯ã?ã®ã¨ãã¾ãã??
            static const auto begin_tag = std::regex(R"(codeSpace *= *")", regex_options);
            // çµäº?¿ã°ã¯ã?å§ã¿ã°ã®æ¬¡ã® "(ã?ãã«ã¯ã©ã¼ã??ã·ã§ã³)ã§ãã??
            static const auto end_tag = std::regex(R"(")", regex_options);
            static const auto begin_tag_hint = "codeSpace";
            auto found_strings = searchAllStringsBetween(begin_tag, end_tag, file_content, begin_tag_hint, "\"", 5, 10);
            return found_strings;
        }

        /**
         * å¼æ°ã® set ã®ä¸­èº«ãç¸å¯¾ãã¹ã¨è§£éãã? setã®å?¦ç´?ãã³ãã?ãã¾ãã??
         * ç¸å¯¾ãã¹ã®åºæºã? ã³ãã?å?? å¼æ° src_base_pathã? ã³ãã?åã? dest_base_path ã«ãªãã¾ãã??
         * ã³ãã?åã«ååã®ãã©ã«ã?ãå­å¨ããå ´åã?ã³ãã?ãã¾ããã?
         * ã³ãã?å?å®å¨ããªã??´åã?ã³ãã?ãã¾ããã?
         */
        void copyFiles(const std::set<std::string>& path_set, const fs::path& src_base_path, const fs::path& dest_base_path){
            for(const auto& path : path_set){
                auto src = src_base_path;
                auto dest = dest_base_path;
                src.append(path).make_preferred();
                dest.append(path).make_preferred();
                if(!fs::exists(src)){
                    std::cout << "file not exist : " << src.string() << std::endl;
                    continue;
                }
                fs::create_directories(dest.parent_path());
                fs::copy(src, dest, fs::copy_options::skip_existing);
            }
        }
    } // fetch ã§ä½¿ã?¡åé¢æ°


    std::string UdxFileCollection::fetch(const std::string& destination_root_path, const GmlFileInfo& gml_file) const {
        const auto root_folder_name = fs::u8path(udx_path_).parent_path().filename().string();
        auto destination_root = fs::u8path(destination_root_path);
        const auto destination_udx_path = destination_root.append(root_folder_name).append("udx").string();
        fs::path gml_destination_path(destination_udx_path);
        gml_destination_path.append(getRelativePath(gml_file.getPath()));
        fs::create_directories(gml_destination_path.parent_path());
        const auto& gml_file_path = gml_file.getPath();
        fs::copy(gml_file_path, gml_destination_path, fs::copy_options::skip_existing);

        // GMLãã¡ã¤ã«ãèª­ã¿è¾¼ã¿ã?¢é£ããã?¯ã¹ãã£ãã¹ã¨ã³ã¼ããªã¹ããã¹ãåå¾ãã¾ãã??
        const auto gml_content = loadFile(gml_file.getPath());
        auto image_paths = searchAllImagePathsInGML(gml_content);
        auto codelist_paths = searchAllCodelistPathsInGML(gml_content);

        for(const auto& path : image_paths){
            std::cout << path << std::endl;
        }
        for(const auto& path : codelist_paths){
            std::cout << path << std::endl;
        }

        // ã?¯ã¹ãã£ã¨ã³ã¼ããªã¹ããã¡ã¤ã«ãã³ãã?ãã¾ãã??
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
        for(const auto& mesh_code : mesh_codes){
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
        if(files_.count(sub_folder) <= 0){
            files_.emplace(sub_folder, std::vector<GmlFileInfo>());
        }
        files_.at(sub_folder).push_back(gml_file_info);
    }

    void UdxFileCollection::setUdxPath(std::string udx_path) {
        udx_path_ = std::move(udx_path);
    }
}
