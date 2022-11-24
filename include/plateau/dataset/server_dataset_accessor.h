#pragma once
#include <plateau/dataset/i_dataset_accessor.h>
#include <plateau/network/client.h>

namespace plateau::dataset{
    /**
     * \brief サーバー上のPLATEAUの3D都市モデルデータ製品へのアクセスを提供します。
     */
    class LIBPLATEAU_EXPORT ServerDatasetAccessor : public IDatasetAccessor{
    public:
        ServerDatasetAccessor();
        std::vector<network::DatasetMetadataGroup> getDatasetMetadataGroup() const;
        /// 上記メソッドのP/Invoke版です。
        void getDatasetMetadataGroup(std::vector<network::DatasetMetadataGroup>& out_group) const;
        void setDatasetID(const std::string& dataset_id);

        /// このメソッドの実行前に、setDatasetID が実行されていることが前提です。
        std::vector<MeshCode> getMeshCodes() override;
        /// 上記メソッドのP/Invoke版です。
        void getMeshCodes(std::vector<MeshCode>& mesh_codes) override;

        std::vector<GmlFile> getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package) override;
        void getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package,
                         std::vector<GmlFile>& out_gml_files) override;
        int getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package) override;
        PredefinedCityModelPackage getPackages() override;

    private:
        plateau::network::Client client_;
        std::string dataset_id_;
    };
}
