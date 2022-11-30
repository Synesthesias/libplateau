#pragma once

#include <plateau/dataset/i_dataset_accessor.h>
#include <plateau/network/client.h>

namespace plateau::dataset {
    /**
     * \brief サーバー上のPLATEAUデータセットに対して、データの検索等を行います。
     */
    class LIBPLATEAU_EXPORT ServerDatasetAccessor : public IDatasetAccessor {
    public:
//        ServerDatasetAccessor();
        explicit ServerDatasetAccessor(const std::string& dataset_id);

        /**
         * \brief データセットグループの一覧をサーバーに問い合わせて返します。
         * 「データセットグループの一覧」とは通常は都道府県の一覧となり、
         * データセットグループの下にデータセットの一覧があります。
         */
        std::vector<network::DatasetMetadataGroup> getDatasetMetadataGroup() const;
        /// 上記メソッドのP/Invoke版です。
        void getDatasetMetadataGroup(std::vector<network::DatasetMetadataGroup>& out_group) const;

        void setDatasetID(const std::string& dataset_id);

        /**
         * データセットに含まれるメッシュコードの一覧をサーバーに問い合わせて返します。
         * このメソッドの実行前に、コンストラクタまたは setDatasetID で
         * データセットIDがセットされていること前提です。
         * TODO こういう但し書きが必要な時点でスマートでない。コンストラクタで setDatasetIDの指定を強制すれば良いのでは。
         */
        std::vector<MeshCode> getMeshCodes() override;
        /// 上記メソッドのP/Invoke版です。
        void getMeshCodes(std::vector<MeshCode>& mesh_codes) override;

        ///
        std::vector<GmlFile> getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package) override;
        /// 上記メソッドのP/Invoke版です。
        void getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package,
                         std::vector<GmlFile>& out_gml_files) override;
        int getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package) override;

        /**
         * package_to_gmls_map_ に含まれるパッケージをEnumフラグ形式で返します。
         * 注意: getGmlFiles の後でないと package_to_gmls_to_map が構築されないので None が返ります。
         * getGmlFiles したことのある Extent に関して利用可能なパッケージを返します。
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

        /**
         * メッシュコードに対応するデータファイルのURLのリストをサーバーに問い合わせ、
         * 結果を package_to_gmls_map_ に追加します。
         */
        void addUrls(const std::vector<MeshCode>& mesh_codes);
    };
}
