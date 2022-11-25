#include <plateau/dataset/server_dataset_accessor.h>
#include <plateau/dataset/local_dataset_accessor.h>

namespace plateau::dataset {
    using namespace plateau::network;

    ServerDatasetAccessor::ServerDatasetAccessor() :
            client_(Client("https://9tkm2n.deta.dev")),// モックサーバーのURL
            cached_mesh_codes_are_valid_(false){
    }

    ServerDatasetAccessor::ServerDatasetAccessor(const std::string& dataset_id) :
        ServerDatasetAccessor(){
        setDatasetID(dataset_id);
    }

    std::vector<DatasetMetadataGroup> ServerDatasetAccessor::getDatasetMetadataGroup() const{
        return client_.getMetadata();
    }

    void ServerDatasetAccessor::getDatasetMetadataGroup(std::vector<network::DatasetMetadataGroup>& out_group) const{
        out_group = getDatasetMetadataGroup();
    }

    void ServerDatasetAccessor::setDatasetID(const std::string& dataset_id) {
        // データセットが変わるということは、キャッシュが古くなるということです。
        if(dataset_id_ != dataset_id) cached_mesh_codes_are_valid_ = false;
        dataset_id_ = dataset_id;
    }

    std::vector<MeshCode> ServerDatasetAccessor::getMeshCodes() {
        if(cached_mesh_codes_are_valid_){
            // すでに取得済みの場合
            return cached_mesh_codes_;
        }
        if(dataset_id_.empty()){
            return {};
        }
        auto mesh_codes = client_.getMeshCodes(dataset_id_);
        cached_mesh_codes_ = mesh_codes;
        cached_mesh_codes_are_valid_ = true;
        return cached_mesh_codes_;
    }

    void ServerDatasetAccessor::getMeshCodes(std::vector<MeshCode>& mesh_codes) {
        mesh_codes = getMeshCodes();
    }

    void ServerDatasetAccessor::addUrls(const std::vector<MeshCode>& mesh_codes){
        for(const auto& mesh_code : mesh_codes){
            // 判明済みのメッシュコードはスキップします。
            if(cached_mesh_codes_are_valid_ &&
            (mesh_codes_included_in_map_.find(mesh_code) != mesh_codes_included_in_map_.cend())){
                continue;
            }

            auto udx_sub_dir_to_urls_map = client_.getFiles({mesh_code});
            for(const auto& [udx_sub_dir, urls] : *udx_sub_dir_to_urls_map){
                auto package = UdxSubFolder::getPackage(udx_sub_dir);
                for(const auto& url : urls){
                    if(package_to_gmls_map_.find(package) != package_to_gmls_map_.cend()){
                        package_to_gmls_map_[package] = std::vector<GmlFile>();
                    }
                    package_to_gmls_map_[package].emplace_back(url);
                }
            }
            mesh_codes_included_in_map_.insert(mesh_code);
        }

    }

    std::vector<GmlFile> ServerDatasetAccessor::getGmlFiles(
            geometry::Extent extent, PredefinedCityModelPackage package) {
        // extent と交わるメッシュコードを求めます。
        auto all_mesh_codes = getMeshCodes();
        auto target_mesh_codes = std::vector<MeshCode>();
        for(const auto& mesh_code : all_mesh_codes){
            if(mesh_code.getExtent().intersects2D(extent)){
                target_mesh_codes.push_back(mesh_code);
            }
        }
        addUrls(target_mesh_codes);
        if(package_to_gmls_map_.find(package) == package_to_gmls_map_.cend()){
            return {};
        }
        return package_to_gmls_map_[package];
    }

    void ServerDatasetAccessor::getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package,
                                            std::vector<GmlFile>& out_gml_files) {
        out_gml_files = getGmlFiles(extent, package);
    }

    int ServerDatasetAccessor::getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package) {
        // TODO 未実装
        return 0;
    }

    PredefinedCityModelPackage ServerDatasetAccessor::getPackages() {
        auto result = PredefinedCityModelPackage::None;
        for (const auto& [key, _] : package_to_gmls_map_) {
            result = result | key;
        }
        return result;
    }


}

