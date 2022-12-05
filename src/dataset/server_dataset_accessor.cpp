#include <plateau/dataset/server_dataset_accessor.h>
#include <plateau/dataset/local_dataset_accessor.h>
#include <plateau/network/network_config.h>
#include "plateau/geometry/geo_coordinate.h"

namespace plateau::dataset {
    using namespace plateau::network;

    ServerDatasetAccessor::ServerDatasetAccessor(const std::string& dataset_id) :
            client_(Client(NetworkConfig::mockServerUrl())),
            cached_mesh_codes_are_valid_(false) {
        setDatasetID(dataset_id);
    }

    std::vector<DatasetMetadataGroup> ServerDatasetAccessor::getDatasetMetadataGroup() const {
        return client_.getMetadata();
    }

    void ServerDatasetAccessor::getDatasetMetadataGroup(std::vector<network::DatasetMetadataGroup>& out_group) const {
        out_group = getDatasetMetadataGroup();
    }

    void ServerDatasetAccessor::setDatasetID(const std::string& dataset_id) {
        // データセットが変わるということは、キャッシュが古くなるということです。
        if (dataset_id_ != dataset_id) cached_mesh_codes_are_valid_ = false;
        dataset_id_ = dataset_id;
    }

    std::vector<MeshCode> ServerDatasetAccessor::getMeshCodes() {
        if (cached_mesh_codes_are_valid_) {
            // すでに取得済みの場合
            return cached_mesh_codes_;
        }
        if (dataset_id_.empty()) {
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

    void ServerDatasetAccessor::addUrls(const std::vector<MeshCode>& mesh_codes) {
        for (const auto& mesh_code: mesh_codes) {
            // 判明済みのメッシュコードはスキップします。
            if (cached_mesh_codes_are_valid_ &&
                (mesh_codes_included_in_map_.find(mesh_code) != mesh_codes_included_in_map_.cend())) {
                continue;
            }

            auto udx_sub_dir_to_urls_map = client_.getFiles({mesh_code});
            for (const auto& [udx_sub_dir, urls]: *udx_sub_dir_to_urls_map) {
                auto package = UdxSubFolder::getPackage(udx_sub_dir);
                for (const auto& [max_lod, url]: urls) {
                    if (package_to_gmls_map_.find(package) == package_to_gmls_map_.cend()) {
                        package_to_gmls_map_.insert(std::make_pair(package, LodGmlPairs()));
                    }
                    LodGmlPairs& lod_gml_pairs = package_to_gmls_map_.at(package);
                    lod_gml_pairs.push_back(std::make_pair(max_lod, GmlFile(url)));
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
        for (const auto& mesh_code: all_mesh_codes) {
            if (mesh_code.getExtent().intersects2D(extent)) {
                target_mesh_codes.push_back(mesh_code);
            }
        }
        // メッシュコードに対応するGMLを問い合わせてキャッシュに追加します。
        addUrls(target_mesh_codes);
        if (package_to_gmls_map_.find(package) == package_to_gmls_map_.cend()) {
            return {};
        }

        const LodGmlPairs& pairs = package_to_gmls_map_.at(package);
        // pairs のうち GmlFile のみを取り出して戻り値とします。
        auto ret = std::vector<GmlFile>();
        for(const auto& [max_lod, gml_file] : pairs) {
            ret.push_back(gml_file);
        }
        return ret;
    }

    void ServerDatasetAccessor::getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package,
                                            std::vector<GmlFile>& out_gml_files) {
        out_gml_files = getGmlFiles(extent, package);
    }

    int ServerDatasetAccessor::getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package) {
        auto center = mesh_code.getExtent().centerPoint();
        auto center_extent = geometry::Extent(center, center);
        auto gmls = getGmlFiles(center_extent, package);
        int max_lod = -1;
        if(package_to_gmls_map_.find(package) == package_to_gmls_map_.end()){
            return -1;
        }
        const LodGmlPairs& pairs = package_to_gmls_map_.at(package);
        for(const auto& pair : pairs){
            max_lod = std::max(max_lod, (int)pair.first);
        }
        return max_lod;
    }

    PredefinedCityModelPackage ServerDatasetAccessor::getPackages() {
        auto result = PredefinedCityModelPackage::None;
        for (const auto& [key, _]: package_to_gmls_map_) {
            result = result | key;
        }
        return result;
    }


}

