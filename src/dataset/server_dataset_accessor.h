#pragma once

#include <plateau/dataset/i_dataset_accessor.h>
#include <plateau/network/client.h>

namespace plateau::dataset {
    /**
     * \brief サーバー上のPLATEAUデータセットに対して、データの検索等を行います。
     */
    class LIBPLATEAU_EXPORT ServerDatasetAccessor : public IDatasetAccessor {
    public:
        /**
         * コンストラクト時にデータセットIDを指定します。
         * このIDはサーバーにデータセット一覧を問い合わせて得られる文字列です。
         */
        explicit ServerDatasetAccessor(const std::string& dataset_id, const network::Client& client);

        void loadFromServer();

        std::set<MeshCode>& getMeshCodes() override;
        std::shared_ptr<std::vector<GmlFile>> getGmlFiles(const PredefinedCityModelPackage package) override;
        void getGmlFiles(const PredefinedCityModelPackage package, std::vector<GmlFile>& out_gml_files) override;

        TVec3d calculateCenterPoint(const plateau::geometry::GeoReference& geo_reference) override;
        
        /**
         * package_to_gmls_map_ に含まれるパッケージをEnumフラグ形式で返します。
         * 注意: getGmlFiles の後でないと package_to_gmls_to_map が構築されないので None が返ります。
         * getGmlFiles したことのある Extent に関して利用可能なパッケージを返します。
         */
        PredefinedCityModelPackage getPackages() override;

        void filter(const geometry::Extent& extent, IDatasetAccessor& collection) const override;
        std::shared_ptr<IDatasetAccessor> filter(const geometry::Extent& extent) const override;
        void filterByMeshCodes(const std::vector<MeshCode>& mesh_codes, IDatasetAccessor& collection) const override;
        std::shared_ptr<IDatasetAccessor> filterByMeshCodes(const std::vector<MeshCode>& mesh_codes) const override;

    private:
        network::Client client_;
        std::string dataset_id_;
        network::DatasetFiles dataset_files_;
        std::set<MeshCode> mesh_codes_;

        void addFile(const std::string& sub_folder, const network::DatasetFileItem& gml_file_info);
    };
}
