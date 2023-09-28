#include <plateau/network/client.h>
#include <plateau/geometry/geo_coordinate.h>
#include <plateau/geometry/geo_reference.h>

#include "server_dataset_accessor.h"

namespace plateau::dataset {
    using namespace network;

    ServerDatasetAccessor::ServerDatasetAccessor(const std::string& dataset_id, const Client& client)
        : client_(client)
        , dataset_id_(dataset_id) {
    }

    void ServerDatasetAccessor::loadFromServer() {
        // データセット情報を再取得します。
        dataset_files_ = client_.getFiles(dataset_id_);
        mesh_codes_.clear();
    }

    std::set<MeshCode>& ServerDatasetAccessor::getMeshCodes() {
        if (mesh_codes_.empty()) {
            for (const auto& [_, files] : dataset_files_) {
                for (const auto& file : files) {
                    mesh_codes_.insert(MeshCode(file.mesh_code));
                }
            }
        }
        return mesh_codes_;
    }

    std::shared_ptr<std::vector<GmlFile>> ServerDatasetAccessor::getGmlFiles(
        const PredefinedCityModelPackage package) {

        auto result = std::make_shared<std::vector<GmlFile>>();
        getGmlFiles(package, *result);
        return result;
    }

    void ServerDatasetAccessor::getGmlFiles(
        const PredefinedCityModelPackage package_flags,
        std::vector<GmlFile>& out_gml_files) {

        auto arg_package_num = static_cast<std::underlying_type<PredefinedCityModelPackage>::type>(package_flags);
        for (const auto& [sub_folder, gml_files] : dataset_files_) {
            auto data_package_num = static_cast<std::underlying_type<PredefinedCityModelPackage>::type>(UdxSubFolder::getPackage(sub_folder));
            if ((data_package_num & arg_package_num) != 0) { // フラグ群の積集合が1つ以上あるなら、引数の package のどれかに合致するので追加
                for (const auto& dataset_file : gml_files) {
                    out_gml_files.emplace_back(dataset_file.url, client_, dataset_file.max_lod);
                }
            }
        }
    }

    TVec3d ServerDatasetAccessor::calculateCenterPoint(const geometry::GeoReference& geo_reference) {

        double lat_sum = 0;
        double lon_sum = 0;
        double height_sum = 0;
        for (const auto& mesh_code : getMeshCodes()) {
            const auto& center = mesh_code.getExtent().centerPoint();
            lat_sum += center.latitude;
            lon_sum += center.longitude;
            height_sum += center.height;
        }
        auto num = (double)getMeshCodes().size();
        geometry::GeoCoordinate geo_average = geometry::GeoCoordinate(lat_sum / num, lon_sum / num, height_sum / num);
        auto euclid_average = geo_reference.project(geo_average);
        return euclid_average;
    }

    PredefinedCityModelPackage ServerDatasetAccessor::getPackages() {
        auto result = PredefinedCityModelPackage::None;
        for (const auto& [key, _] : dataset_files_) {
            result = result | UdxSubFolder::getPackage(key);
        }
        return result;
    }

    void ServerDatasetAccessor::addFile(const std::string& sub_folder, const DatasetFileItem& gml_file_info) {
        if (dataset_files_.find(sub_folder) == dataset_files_.end()) {
            dataset_files_.emplace(sub_folder, std::vector<DatasetFileItem>());
        }
        dataset_files_[sub_folder].push_back(gml_file_info);
    }

    std::shared_ptr<IDatasetAccessor> ServerDatasetAccessor::filter(const geometry::Extent& extent) const {
        auto result = std::make_shared<ServerDatasetAccessor>(dataset_id_, client_);
        filter(extent, *result);
        return result;
    }

    void ServerDatasetAccessor::filter(const geometry::Extent& extent_filter, IDatasetAccessor& collection) const {
        const auto out_collection_ptr = dynamic_cast<ServerDatasetAccessor*>(&collection);
        if (out_collection_ptr == nullptr)
            return;

        for (const auto& [package, files] : dataset_files_) {
            for (const auto& file : files) {
                auto extent = MeshCode(file.mesh_code).getExtent();
                if (extent_filter.intersects2D(extent)) {
                    out_collection_ptr->addFile(package, file);
                }
            }
        }
    }

    void ServerDatasetAccessor::filterByMeshCodes(const std::vector<MeshCode>& mesh_codes,
                                                 IDatasetAccessor& collection) const {
        const auto out_collection_ptr = dynamic_cast<ServerDatasetAccessor*>(&collection);
        if (out_collection_ptr == nullptr)
            return;

        // 検索用に、引数の mesh_codes を文字列のセットにします。
        auto mesh_codes_str_set = std::set<std::string>();
        for (auto mesh_code : mesh_codes) {
            // 各地域メッシュについて上位の地域メッシュも含め登録する。
            // 重複する地域メッシュはinsert関数で弾かれる。
            for (; mesh_code.getLevel() >= 2; mesh_code = mesh_code.upper()) {
                if (!mesh_code.isValid())
                    break;
                mesh_codes_str_set.insert(mesh_code.get());
            }
        }

        // ファイルごとに mesh_codes_str_set に含まれるなら追加していきます。
        for (const auto& [sub_folder, files] : dataset_files_) {
            for (const auto& file : files) {
                if (mesh_codes_str_set.find(file.mesh_code) != mesh_codes_str_set.end()) {
                    out_collection_ptr->addFile(sub_folder, file);
                }
            }
        }
    }

    std::shared_ptr<IDatasetAccessor>
        ServerDatasetAccessor::filterByMeshCodes(const std::vector<MeshCode>& mesh_codes) const {
        auto result = std::make_shared<ServerDatasetAccessor>(dataset_id_, client_);
        filterByMeshCodes(mesh_codes, *result);
        return result;
    }
}
