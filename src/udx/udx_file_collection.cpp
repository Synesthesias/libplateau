#include <filesystem>
#include <utility>

#include <plateau/udx/udx_file_collection.h>
#include <fstream>
#include <queue>
#include <set>
#include <regex>

namespace plateau::udx {
    namespace fs = std::filesystem;

    PredefinedCityModelPackage UdxSubFolder::getPackage() const {
        if (name_ == bldg) return PredefinedCityModelPackage::Building;
        if (name_ == tran) return PredefinedCityModelPackage::Road;
        if (name_ == urf) return PredefinedCityModelPackage::UrbanPlanningDecision;
        if (name_ == luse) return PredefinedCityModelPackage::LandUse;
        if (name_ == frn) return PredefinedCityModelPackage::CityFurniture;
        if (name_ == veg) return PredefinedCityModelPackage::Vegetation;
        if (name_ == dem) return PredefinedCityModelPackage::Relief;
        if (name_ == fld) return PredefinedCityModelPackage::DisasterRisk;
        if (name_ == tnm) return PredefinedCityModelPackage::DisasterRisk;
        if (name_ == lsld) return PredefinedCityModelPackage::DisasterRisk;
        if (name_ == htd) return PredefinedCityModelPackage::DisasterRisk;
        if (name_ == ifld) return PredefinedCityModelPackage::DisasterRisk;
        return PredefinedCityModelPackage::Unknown;
    }

    CityModelPackageInfo UdxSubFolder::getPackageInfo() const {
        return CityModelPackageInfo::getPredefined(getPackage());
    }

    std::shared_ptr<UdxFileCollection> UdxFileCollection::find(const std::string& source) {
        auto result = std::make_shared<UdxFileCollection>();
        find(source, *result);
        return result;
    }

    namespace{
        /**
         * 指定パスを起点に、幅優先探索(BFS)でGMLファイルを検索します。
         * 検索の高速化のため、GMLファイルの配置場所の深さはすべて同じであるという前提に立ち、
         * 最初のGMLファイルが見つかった地点でこれ以上深いフォルダの探索は中止します。
         * 同じ深さにある別のフォルダは探索対象とします。
         * @param dir_path  検索の起点となるパスです。
         * @param result 結果はこの vector に追加されます。
         * @return GMLファイルのパスの vector です。
         */
        void findGMLsBFS(const std::string& dir_path, std::vector<GmlFileInfo>& result){
            auto queue = std::queue<std::string>();
            queue.push(dir_path);
            bool push_more_dir = true;
            while(!queue.empty()){
                auto next_dir = queue.front();
                queue.pop();
                // ファイルから検索します。
                for(const auto& entry : fs::directory_iterator(next_dir)){
                    if(entry.is_directory()) continue;
                    const auto& path = entry.path();
                    if(path.extension() == ".gml"){
                        result.emplace_back(path.string());
                        // 最初のGMLファイルが見つかったら、これ以上探索キューに入れないようにします。
                        // 同じ深さにあるフォルダはすでにキューに入っているので、「深さは同じだけどフォルダが違う」という状況は検索対象に含まれます。
                        push_more_dir = false;
                    }
                }
                // 次に探索すべきディレクトリをキューに入れます。
                if(!push_more_dir) continue;
                for(const auto& entry : fs::directory_iterator(next_dir)){
                    if(!entry.is_directory()) continue;
                    std::cout << "queued " << entry.path().string() << std::endl;
                    queue.push(entry.path().string());
                }
            }
        }
    }

    void UdxFileCollection::find(const std::string& source, UdxFileCollection& collection) {
        collection.udx_path_ = fs::u8path(source).append(u"udx").make_preferred().u8string();
        // udxフォルダ内の各フォルダについて
        for (const auto& entry : fs::directory_iterator(collection.udx_path_)) {
            const auto package = UdxSubFolder(entry.path().filename().string()).getPackage();
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
                        collection.files_[sub_folder].push_back(file);
                    }
                }
            }
        }
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

        return files_[package].size();
    }

    std::shared_ptr<std::vector<std::string>> UdxFileCollection::getGmlFiles(PredefinedCityModelPackage package) {
        if (files_.find(package) == files_.end())
            throw std::out_of_range("Key not found");

        const auto result = std::make_shared<std::vector<std::string>>();
        for (const auto file : files_[package]) {
            result->push_back(file.getPath());
        }
        return result;
    }

    // fetch で使う無名関数
    namespace{
        /**
         * 引数文字列 str のうち、引数 begin_tag と end_tag で囲まれた文字列をすべて検索し set で返します。
         * end_tag は begin_tag が登場する箇所より後が検索対象となります。
         * begin_tag に対応する end_tag がない場合、strの末尾までが対象となります。
         * 検索結果のうち同じ文字列は1つにまとめられます。
         */
        std::set<std::string> searchAllStringsBetween(const std::regex& begin_tag, const std::regex& end_tag, const std::string& str, const std::string& begin_tag_hint){
            std::set<std::string> found;
            std::smatch begin_tag_matched;
            auto search_start_for_begin_tag = str.begin();
            while(true){ // TODO
                auto hint_matched = str.find(begin_tag_hint, search_start_for_begin_tag - str.begin());
                if(hint_matched == std::string::npos) break;
                search_start_for_begin_tag = str.begin() + std::max((long long)0, (long long)hint_matched - 10);
                bool is_found = std::regex_search(search_start_for_begin_tag, str.end(), begin_tag_matched, begin_tag);
                if(!is_found) break;
                const auto next_of_begin_tag = begin_tag_matched[0].second;
                std::smatch end_tag_matched;
                decltype(end_tag_matched[0].first) end_tag_iter;
                if(std::regex_search(next_of_begin_tag, str.end(), end_tag_matched, end_tag)){
                    end_tag_iter = end_tag_matched[0].first;
                }else{
                    end_tag_iter = str.end();
                }
                const auto next_of_end_tag = end_tag_matched[0].second;
                search_start_for_begin_tag = next_of_end_tag;
                found.insert(std::string(next_of_begin_tag, end_tag_iter));
            }
            return found;
        }

        /**
         * 上の searchAllStringsBetween 関数について、探索対象が文字列の代わりにファイルになった版です。
         */
        std::set<std::string> searchAllStringsBetweenTagInFile(const std::regex& begin_tag, const std::regex& end_tag, const fs::path& file_path, const std::string& begin_tag_hint){
            std::ifstream ifs(file_path.u8string());
            if(!ifs){
                throw std::runtime_error("searchAllStringsBetweenTagInFile : Could not open file " + file_path.u8string());
            }
            std::stringstream buffer;
            buffer << ifs.rdbuf();
            auto file_content = buffer.str();
            auto founds = searchAllStringsBetween(begin_tag, end_tag, file_content, begin_tag_hint);
            return founds;
        }

        std::set<std::string> searchAllImagePathsInGML(const fs::path& gml_path){
            // 開始タグは <app:imageURI> です。ただし、<括弧> の前後に空白文字があっても良いものとします。
            auto begin_tag = std::regex(R"(<\s*app:imageURI\s*>\s*)");
            // 終了タグは </app:imageURI> です。ただし、<括弧> と /(スラッシュ) の前後に空白文字があっても良いものとします。
            auto end_tag = std::regex(R"(<\s*/\s*app:imageURI\s*>)");
            const auto begin_tag_hint = std::string("app:imageURI");
            auto found_url_strings = searchAllStringsBetweenTagInFile(begin_tag, end_tag, gml_path, begin_tag_hint);
            return found_url_strings;
        }

        std::set<std::string> searchAllCodelistPathsInGML(const fs::path& gml_path){
            // 開始タグは codeSpace=" です。ただし =(イコール), "(ダブルクォーテーション)の前後に空白文字があっても良いものとします。
            auto begin_tag = std::regex(R"(codeSpace\s*=\s*["']\s*)");
            // 終了タグは、開始タグの次の "(ダブルクォーテーション)です。前後に空白があっても良いものとします。
            auto end_tag = std::regex(R"(\s*")");
            const auto begin_tag_hint = "codeSpace";
            auto found_strings = searchAllStringsBetweenTagInFile(begin_tag, end_tag, gml_path, begin_tag_hint);
            return found_strings;
        }

        /**
         * 引数の set の中身を相対パスと解釈し、 setの各要素をコピーします。
         * 相対パスの基準は コピー元は 引数 src_base_path、 コピー先は dest_base_path になります。
         * コピー先に同名のフォルダが存在する場合はコピーしません。
         * コピー元が実在しない場合はコピーしません。
         */
        void copySet(const std::set<std::string>& path_set, const fs::path& src_base_path, const fs::path& dest_base_path){
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
    } // fetch で使う無名関数


    std::string UdxFileCollection::fetch(const std::string& destination_root_path, const GmlFileInfo& gml_file) const {
        const auto root_folder_name = fs::u8path(udx_path_).parent_path().filename().string();
        auto destination_root = fs::u8path(destination_root_path);
        const auto destination_udx_path = destination_root.append(root_folder_name).append("udx").string();
        fs::path gml_destination_path(destination_udx_path);
        gml_destination_path.append(getRelativePath(gml_file.getPath()));
        fs::create_directories(gml_destination_path.parent_path());
        const auto& gml_file_path = gml_file.getPath();
        fs::copy(gml_file_path, gml_destination_path, fs::copy_options::skip_existing);
        auto image_paths = searchAllImagePathsInGML(gml_file.getPath());
        auto codelist_paths = searchAllCodelistPathsInGML(gml_file.getPath());

        for(const auto& path : image_paths){
            std::cout << path << std::endl;
        }
        for(const auto& path : codelist_paths){
            std::cout << path << std::endl;
        }

        auto gml_dir_path = fs::path(gml_file_path).parent_path();
        auto app_destination_path = fs::path(destination_udx_path).append(getRelativePath(gml_dir_path.string()));
        copySet(image_paths, gml_dir_path, app_destination_path);
        copySet(codelist_paths, gml_dir_path, app_destination_path);

        return gml_destination_path.u8string();
    }

    std::string UdxFileCollection::getU8RelativePath(const std::string& path) const {
        return fs::relative(fs::u8path(path), fs::u8path(udx_path_)).u8string();
    }

    std::string UdxFileCollection::getRelativePath(const std::string& path) const {
        return fs::relative(fs::u8path(path), fs::u8path(udx_path_)).string();
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
}
