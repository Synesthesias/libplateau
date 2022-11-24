#include <plateau/dataset/server_dataset_accessor.h>

namespace plateau::dataset {
    using namespace plateau::network;

    ServerDatasetAccessor::ServerDatasetAccessor() :
            client_(Client()) {
        // モックサーバーのURL
        client_.setApiServerUrl("https://9tkm2n.deta.dev");
    }

    std::vector<DatasetMetadataGroup> ServerDatasetAccessor::getDatasetMetadataGroup() const{
        return client_.getMetadata();
    }

    void ServerDatasetAccessor::getDatasetMetadataGroup(std::vector<network::DatasetMetadataGroup>& out_group) const{
        out_group = getDatasetMetadataGroup();
    }

    void ServerDatasetAccessor::setDatasetID(const std::string& dataset_id) {
        dataset_id_ = dataset_id;
    }

    std::vector<MeshCode> ServerDatasetAccessor::getMeshCodes() {
        if(dataset_id_.empty()){
            return {};
        }
        auto mesh_codes = client_.getMeshCodes(dataset_id_);
        return mesh_codes;
    }

    void ServerDatasetAccessor::getMeshCodes(std::vector<MeshCode>& mesh_codes) {
        mesh_codes = getMeshCodes();
    }

    std::vector<GmlFile>
    ServerDatasetAccessor::getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package) {
        // TODO 未実装
        return std::vector<GmlFile>();
    }

    void ServerDatasetAccessor::getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package,
                                            std::vector<GmlFile>& out_gml_files) {
        // TODO 未実装
    }

    int ServerDatasetAccessor::getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package) {
        // TODO 未実装
        return 0;
    }

    PredefinedCityModelPackage ServerDatasetAccessor::getPackages() {
        // TODO 未実装
        return PredefinedCityModelPackage::Road;
    }


}

