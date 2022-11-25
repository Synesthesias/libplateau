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

        /**
         * メッシュコードに対応するデータファイルのURLのリストをサーバーに問い合わせ、
         * 結果を package_to_gmls_map_ に追加します。
         */
        void addUrls(const std::vector<MeshCode>& mesh_codes);

        std::vector<GmlFile> getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package) override;
        void getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package,
                         std::vector<GmlFile>& out_gml_files) override;
        int getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package) override;
        /**
         * package_to_gmls_map_ に含まれるパッケージをEnumフラグ形式で返します。
         * すなわち、getGmlFiles したことのある Extent に関して利用可能なパッケージを返します。
         */
        PredefinedCityModelPackage getPackages() override;

    private:
        plateau::network::Client client_;
        std::string dataset_id_;
        std::map<PredefinedCityModelPackage, std::vector<GmlFile>> package_to_gmls_map_;
        std::vector<MeshCode> cached_mesh_codes_;
        bool cached_mesh_codes_are_valid_;
        /// package_to_gmls_map_ に含まれる MeshCode を覚えておくことで、無駄なネットワーク利用を抑えます。
        std::set<MeshCode> mesh_codes_included_in_map_;
    };
}
