#include <filesystem>
#include <utility>

#include <plateau/udx/udx_file_collection.h>
#include <fstream>
#include <queue>

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
         * @param dir_path  検索の起点となるパスです。
         * @return GMLファイルのパスの vector です。
         */
        std::vector<std::string> findGmlBFS(const std::string& dir_path){
            auto found = std::vector<std::string>();
            auto queue = std::queue<std::string>();
            queue.push(dir_path);
            bool push_more_dir = true;
            std::cout << "start findGmlBFS" << std::endl;
            while(!queue.empty()){
                auto next_dir = queue.front();
                queue.pop();
                // ファイルから検索します。
                for(const auto& entry : fs::directory_iterator(next_dir)){
                    if(entry.is_directory()) continue;
                    std::cout << "searching file " << entry.path().string() << std::endl;
                    auto path = entry.path();
                    if(path.extension() == ".gml"){
                        found.push_back(path.string());
                        // 最初のGMLファイルが見つかったら、これ以上探索キューに入れないようにします。
                        // 同じ深さにあるフォルダはすでにキューに入っているので、「深さは同じだけどフォルダが違う」という状況は検索対象に含まれます。
                        push_more_dir = false;
                    }
                }
                // 次に探索すべきディレクトリをキューに入れます。
                if(!push_more_dir) continue;
                for(const auto& entry : fs::directory_iterator(next_dir)){
                    std::cout << "searching dir " << entry.path().string() << std::endl;
                    if(!entry.is_directory()) continue;
                    queue.push(entry.path().string());
                }
            }
            return found;
        }
    }

    void UdxFileCollection::find(const std::string& source, UdxFileCollection& collection) {
        collection.udx_path_ = fs::u8path(source).append(u"udx").make_preferred().u8string();
        for (const auto& entry : fs::directory_iterator(collection.udx_path_)) {
//            auto& gml_files = collection.files_[UdxSubFolder(entry.path().filename().string()).getPackage()];
//            auto recursive_iter = fs::recursive_directory_iterator(entry.path());
//            for (const auto& gml_entry : recursive_iter) {
//                if(recursive_iter.depth() > gml_found_depth){
//                    std::cout << "popped" << std::endl;
//                    recursive_iter.pop();
//                }
//                std::cout << "searching " << gml_entry.path() << "  depth " << recursive_iter.depth() <<  std::endl;
//                if (gml_entry.path().extension() == ".gml") {
//                    gml_found_depth = recursive_iter.depth();
//                    const auto gml_path = fs::u8path(gml_entry.path().string()).make_preferred().u8string();
//                    gml_files.emplace_back(gml_path);
//                }
//            }
            auto package = UdxSubFolder(entry.path().filename().string()).getPackage();
            auto& file_map = collection.files_;
            if(file_map.count(package) == 0){
                file_map.emplace(package, std::vector<GmlFileInfo>());
            }
            auto& gml_files = collection.files_.at(package);
            auto found_gml_paths = findGmlBFS(entry.path().string());
            for(const auto& path : found_gml_paths){
                std::cout << path << std::endl;
                gml_files.emplace_back(path);
            }
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

    namespace{
        std::vector<fs::path> searchAllImagePathsInGML(fs::path gml_path){
            std::ifstream ifs(gml_path.u8string());
            if(!ifs){
                throw std::runtime_error("searchAllImagePathsInGML : Could not open file " + gml_path.u8string());
            }
            auto found_paths = std::vector<fs::path>();
            std::string line;
            while(std::getline(ifs, line)){
                auto found_pos = line.find("<app:imageURI>");
                if(found_pos == std::string::npos) continue;
                auto begin_pos = found_pos + strlen("<app:imageURI>");
                auto end_pos = line.find("</app:imageURI>");
                if(end_pos == std::string::npos) end_pos = line.size();
                auto url = line.substr(begin_pos, (end_pos - begin_pos));
                found_paths.push_back(fs::u8path(url));
            }
            return found_paths;
        }
    }


    std::string UdxFileCollection::fetch(const std::string& destination_root_path, const GmlFileInfo& gml_file) const {
        const auto root_folder_name = fs::u8path(udx_path_).parent_path().filename().string();
        auto destination_root = fs::u8path(destination_root_path);
        const auto destination_udx_path = destination_root.append(root_folder_name).append("udx").string();
        fs::path gml_destination_path(destination_udx_path);
        gml_destination_path.append(getRelativePath(gml_file.getPath()));
        fs::create_directories(gml_destination_path.parent_path());
        fs::copy(gml_file.getPath(), gml_destination_path, fs::copy_options::skip_existing);
        // TODO: 取ってくるべきcodelists,textureをgmlファイルから読み取る
        auto image_paths = searchAllImagePathsInGML(gml_file.getPath());
        // TODO 仮
        std::cout << "found image path count : " << image_paths.size() << std::endl;
        for(const auto& path : image_paths){
            std::cout << path.u8string() << std::endl;
        }
        //fs::path app_destination_path(destination_udx_path);
        //app_destination_path.append(getRelativePath(gml_file_path));
        //if (fs::exists(file.getAppearanceDirectoryPath()))
        //    fs::copy(file.getAppearanceDirectoryPath(), app_destination_path, fs::copy_options::recursive | fs::copy_options::update_existing);
            //    const auto root_folder_name = fs::u8path(udx_path_).parent_path().filename().u8string();
    //    const auto source_path = fs::u8path(udx_path_).parent_path().append(u8"codelists");
    //    const auto destination_path = fs::u8path(destination_root_path).append(root_folder_name).append(u8"codelists").u8string();
    //    fs::copy(source_path, destination_path, fs::copy_options::recursive | fs::copy_options::skip_existing);

        return gml_destination_path.u8string();
    }

    std::string UdxFileCollection::getU8RelativePath(const std::string& path) const {
        return fs::relative(fs::u8path(path), fs::u8path(udx_path_)).u8string();
    }

    std::string UdxFileCollection::getRelativePath(const std::string& path) const {
        return fs::relative(fs::u8path(path), fs::u8path(udx_path_)).string();
    }

    std::vector<MeshCode>& UdxFileCollection::getMeshCodes() {
        if (!mesh_codes_.empty())
            return mesh_codes_;

        for (const auto& [_, files] : files_) {
            for (const auto& file : files) {
                if (std::find(mesh_codes_.cbegin(), mesh_codes_.cend(), file.getMeshCode()) == mesh_codes_.cend()) {
                    mesh_codes_.push_back(file.getMeshCode());
                }
            }
        }
        return mesh_codes_;
    }
}
