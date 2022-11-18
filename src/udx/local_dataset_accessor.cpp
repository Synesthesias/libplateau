#include <filesystem>
#include <utility>

#include <plateau/udx/local_dataset_accessor.h>
#include <fstream>
#include <queue>
#include <set>
#include <regex>
#include "plateau/geometry/geo_reference.h"
#include "plateau/udx/lod_searcher.h"

namespace plateau::udx {
    namespace fs = std::filesystem;
    using namespace geometry;

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

    std::shared_ptr<LocalDatasetAccessor> LocalDatasetAccessor::find(const std::string& source) {
        auto result = std::make_shared<LocalDatasetAccessor>();
        find(source, *result);
        return result;
    }

    namespace {
        /**
         * 指定パスを起点に、幅優先探索(BFS)でGMLファイルを検索します。
         * 検索の高速化のため、GMLファイルの配置場所の深さはすべて同じであるという前提に立ち、
         * 最初のGMLファイルが見つかった地点でこれ以上深いフォルダの探索は中止します。
         * 同じ深さにある別のフォルダは探索対象とします。
         * @param dir_path  検索の起点となるパスです。
         * @param result 結果はこの vector に追加されます。
         * @return GMLファイルのパスの vector です。
         */
        void findGMLsBFS(const std::string& dir_path, std::vector<GmlFile>& result) {
            auto queue = std::queue<std::string>();
            queue.push(dir_path);
            bool push_more_dir = true;
            while (!queue.empty()) {
                auto next_dir = queue.front();
                queue.pop();
                // ファイルから検索します
                for (const auto& entry : fs::directory_iterator(next_dir)) {
                    if (entry.is_directory()) continue;
                    const auto& path = entry.path();
                    if (path.extension() == ".gml") {
                        result.emplace_back(path.string());
                        // 最初のGMLファイルが見つかったら、これ以上探索キューに入れないようにします。
                        // 同じ深さにあるフォルダはすでにキューに入っているので、「深さは同じだけどフォルダが違う」という状況は検索対象に含まれます。
                        push_more_dir = false;
                    }
                }
                // 次に探索すべきディレクトリをキューに入れます。
                if (!push_more_dir) continue;
                for (const auto& entry : fs::directory_iterator(next_dir)) {
                    if (!entry.is_directory()) continue;
                    queue.push(entry.path().string());
                }
            }
        }
    }

    void LocalDatasetAccessor::find(const std::string& source, LocalDatasetAccessor& collection) {
        collection.udx_path_ = fs::u8path(source).append(u"udx").make_preferred().u8string();
        // udxフォルダ内の各フォルダについて
        for (const auto& entry : fs::directory_iterator(collection.udx_path_)) {
            if (!entry.is_directory()) continue;
            auto udx_sub_folder = UdxSubFolder(entry.path().filename().string());
            const auto package = udx_sub_folder.getPackage(udx_sub_folder.getName());
            auto& file_map = collection.files_;
            if (file_map.count(package) == 0) {
                file_map.emplace(package, std::vector<GmlFile>());
            }
            auto& gml_files = collection.files_.at(package);
            findGMLsBFS(entry.path().string(), gml_files);
        }
    }

    std::vector<GmlFile> LocalDatasetAccessor::getGmlFiles(Extent extent, PredefinedCityModelPackage package){
        auto filtered = LocalDatasetAccessor();
        filter(extent, filtered);
        auto count = filtered.getGmlFileCount(package);
        auto ret = std::vector<GmlFile>();
        for(int i=0; i<count; i++){
            ret.push_back(filtered.getGmlFile(package, i));
        }
        return ret;
    }

    int LocalDatasetAccessor::getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package) {
        auto mesh_code_filtered = filterByMeshCodes({mesh_code});
        auto count = mesh_code_filtered->getGmlFileCount(package);
        int max_lod = -1;
        for(auto i=0; i<count; i++){
            const auto& gml = mesh_code_filtered->getGmlFile(package, i);
            auto lods = LodSearcher::searchLodsInFile(fs::u8path(gml.getPath()));
            max_lod = std::max(max_lod, lods.getMax());
        }
        return max_lod;
    }

    std::shared_ptr<LocalDatasetAccessor> LocalDatasetAccessor::filter(const geometry::Extent& extent) const {
        auto result = std::make_shared<LocalDatasetAccessor>();
        filter(extent, *result);
        return result;
    }

    void LocalDatasetAccessor::filter(const geometry::Extent& extent, LocalDatasetAccessor& collection) const {
        std::vector<MeshCode> mesh_codes;
        MeshCode::getThirdMeshes(extent, mesh_codes);
        std::set<std::string> second_mesh_strings;

        // 2次メッシュのリスト取得
        for (const auto& mesh_code : mesh_codes) {
            second_mesh_strings.insert(mesh_code.asSecond().get());
        }

        for (const auto& second_mesh_string : second_mesh_strings) {
            mesh_codes.emplace_back(second_mesh_string);
        }

        filterByMeshCodes(mesh_codes, collection);
    }

    void LocalDatasetAccessor::filterByMeshCodes(const std::vector<MeshCode>& mesh_codes,
                                                 LocalDatasetAccessor& collection) const {
        // これがないとフィルターの結果に対して fetch を実行するときにパスがずれます。
        collection.setUdxPath(udx_path_);
        // 検索用に、引数の mesh_codes を文字列のセットにします。
        auto mesh_codes_str_set = std::set<std::string>();
        for (const auto& mesh_code : mesh_codes) {
            mesh_codes_str_set.insert(mesh_code.get());
        }
        // ファイルごとに mesh_codes_str_set に含まれるなら追加していきます。
        for (const auto& [sub_folder, files] : files_) {
            for (const auto& file : files) {
                if (mesh_codes_str_set.find(file.getMeshCode().get()) != mesh_codes_str_set.end()) {
                    collection.addFile(sub_folder, file);
                }
            }
        }
    }

    std::shared_ptr<LocalDatasetAccessor>
        LocalDatasetAccessor::filterByMeshCodes(const std::vector<MeshCode>& mesh_codes) const {
        auto result = std::make_shared<LocalDatasetAccessor>();
        filterByMeshCodes(mesh_codes, *result);
        return result;
    }

    PredefinedCityModelPackage LocalDatasetAccessor::getPackages() {
        auto result = PredefinedCityModelPackage::None;
        for (const auto& [key, _] : files_) {
            result = result | key;
        }
        return result;
    }

    const std::string& LocalDatasetAccessor::getGmlFilePath(PredefinedCityModelPackage package, int index) {
        return getGmlFile(package, index).getPath();
    }

    const GmlFile& LocalDatasetAccessor::getGmlFile(PredefinedCityModelPackage package, int index) {
        if (files_.find(package) == files_.end())
            throw std::out_of_range("Key not found");

        if (index >= files_[package].size())
            throw std::out_of_range("Index out of range");

        return files_[package][index];
    }

    int LocalDatasetAccessor::getGmlFileCount(PredefinedCityModelPackage package) {
        if (files_.find(package) == files_.end())
            throw std::out_of_range("Key not found");

        return (int)files_[package].size();
    }

    std::shared_ptr<std::vector<std::string>> LocalDatasetAccessor::getGmlFiles(PredefinedCityModelPackage package) {
        const auto result = std::make_shared<std::vector<std::string>>();

        if (files_.find(package) == files_.end())
            return result;

        for (const auto file : files_[package]) {
            result->push_back(file.getPath());
        }
        return result;
    }



    std::string LocalDatasetAccessor::getU8RelativePath(const std::string& path) const {
        return fs::relative(fs::u8path(path), fs::u8path(udx_path_)).u8string();
    }

    TVec3d LocalDatasetAccessor::calculateCenterPoint(const geometry::GeoReference& geo_reference) {
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

    std::string LocalDatasetAccessor::getRelativePath(const std::string& path) const {
        return fs::relative(fs::u8path(path).make_preferred(), fs::u8path(udx_path_)).make_preferred().string();
    }

    std::vector<MeshCode> LocalDatasetAccessor::getMeshCodes() {
        if (mesh_codes_.empty()){
            for (const auto& [_, files] : files_) {
                for (const auto& file : files) {
                    mesh_codes_.insert(file.getMeshCode());
                }
            }
        }

        // set を vector に変換します。
        auto vector_mesh_codes = std::vector<MeshCode>();
        for(const auto& mesh_code : mesh_codes_){
            vector_mesh_codes.push_back(mesh_code);
        }
        return vector_mesh_codes;
    }

    void LocalDatasetAccessor::addFile(PredefinedCityModelPackage sub_folder, const GmlFile& gml_file_info) {
        if (files_.count(sub_folder) <= 0) {
            files_.emplace(sub_folder, std::vector<GmlFile>());
        }
        files_.at(sub_folder).push_back(gml_file_info);
    }

    void LocalDatasetAccessor::setUdxPath(std::string udx_path) {
        udx_path_ = std::move(udx_path);
    }
}
