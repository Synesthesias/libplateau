#pragma once

#include "plateau/geometry/geo_reference.h"
#include <plateau/dataset/i_dataset_accessor.h>

namespace plateau::dataset {
    /**
     * \brief ローカルPCに存在するPLATEAUの3D都市モデルデータ製品へのアクセスを提供します。
     */
    class LIBPLATEAU_EXPORT LocalDatasetAccessor : public IDatasetAccessor {
    public:

        /**
         * \brief source内に含まれる3D都市モデルデータを全て取得します。
         * \param source 3D都市モデルデータ製品のルートフォルダ(udx, codelists等のフォルダを含むフォルダ)へのパス
         */
        static std::shared_ptr<LocalDatasetAccessor> find(const std::string& source);

        /**
         * \brief source内に含まれる3D都市モデルデータを全て取得します。
         * \param source 3D都市モデルデータ製品のルートフォルダ(udx, codelists等のフォルダを含むフォルダ)へのパス
         * \param collection 取得されたデータの格納先
         */
        static void find(const std::string& source, LocalDatasetAccessor& collection);

        /**
         * \brief Extent と Package で絞り込んだ GmlInfo の vector を返します。
         */
        //std::shared_ptr<std::vector<GmlFile>> getGmlFiles(const geometry::Extent& extent, const PredefinedCityModelPackage package) override;

        //void getGmlFiles(const geometry::Extent& extent, const PredefinedCityModelPackage package,
        //                 std::vector<GmlFile>& out_gml_files) override;
        
        std::shared_ptr<IDatasetAccessor> filter(const geometry::Extent& extent) const override;
        void filter(const geometry::Extent& extent, IDatasetAccessor& collection) const override;
        void filterByMeshCodes(const std::vector<MeshCode>& mesh_codes, IDatasetAccessor& collection) const override;
        std::shared_ptr<IDatasetAccessor> filterByMeshCodes(const std::vector<MeshCode>& mesh_codes) const override;

        /**
         * \brief 存在する都市モデルパッケージをマスクとして取得します。
         */
        PredefinedCityModelPackage getPackages() override;

        const std::string& getGmlFilePath(PredefinedCityModelPackage package, int index);
        const GmlFile& getGmlFile(PredefinedCityModelPackage package, int index);

        /**
         * \brief packageに該当するCityGMLファイルを取得します。
         * \param package 都市モデルパッケージ
         */
        std::shared_ptr<std::vector<GmlFile>> getGmlFiles(PredefinedCityModelPackage package) override;

        /**
         * \brief packageに該当するCityGMLファイルを取得します。
         *        なければ空のvectorを返します。
         * \param package 都市モデルパッケージ
         * \param gml_files 取得結果の格納先
         */
        void getGmlFiles(PredefinedCityModelPackage package, std::vector<GmlFile>& gml_files) override;

        /**
         * \brief packageに該当するCityGMLファイルの個数を取得します。
         * \param package 都市モデルパッケージ
         */
        int getGmlFileCount(PredefinedCityModelPackage package);

        /**
         * \brief 都市モデルデータが存在する地域メッシュのリストを取得します。
         */
        std::set<MeshCode>& getMeshCodes() override;

        std::string getRelativePath(const std::string& path) const;
        std::string getU8RelativePath(const std::string& path) const;

        /**
         * 各メッシュコードの中心地点の平均を求め、直交座標系で返します。
         */
        TVec3d calculateCenterPoint(const plateau::geometry::GeoReference& geo_reference) override;

        LocalDatasetAccessor* create() const override { return new LocalDatasetAccessor(); }
        LocalDatasetAccessor* clone() const override { return new LocalDatasetAccessor(*this); }

    private:
        std::string udx_path_;
        std::map<PredefinedCityModelPackage, std::vector<GmlFile>> files_;
        std::set<MeshCode> mesh_codes_;
        std::map<std::string, std::vector<GmlFile>> files_by_code_;
        void addFile(PredefinedCityModelPackage sub_folder, const GmlFile& gml_file_info);
        void setUdxPath(std::string udx_path);
    };
}
