﻿#pragma once

#include "plateau/geometry/geo_reference.h"
#include <plateau/udx/i_dataset_accessor.h>

namespace plateau::udx {
    /**
     * \brief PLATEAUの3D都市モデルのフォルダ階層でudxの直下にあるフォルダを表します。
     */
    class LIBPLATEAU_EXPORT UdxSubFolder {
    public:
        UdxSubFolder(std::string name)
            : name_(std::move(name)) {
        }

        //! 建築物、建築物部分、建築物付属物及びこれらの境界面
        inline static const std::string bldg = "bldg";
        //! 道路
        inline static const std::string tran = "tran";
        //! 都市計画決定情報
        inline static const std::string urf = "urf";
        //! 土地利用
        inline static const std::string luse = "luse";
        //! 都市設備
        inline static const std::string frn = "frn";
        //! 植生
        inline static const std::string veg = "veg";
        //! 起伏
        inline static const std::string dem = "dem";
        //! 洪水浸水想定区域
        inline static const std::string fld = "fld";
        //! 津波浸水想定
        inline static const std::string tnm = "tnm";
        //! 土砂災害警戒区域
        inline static const std::string lsld = "lsld";
        //! 高潮浸水想定区域
        inline static const std::string htd = "htd";
        //! 内水浸水想定区域
        inline static const std::string ifld = "ifld";

        static PredefinedCityModelPackage getPackage(const std::string& folder_name);
        static CityModelPackageInfo getPackageInfo(const std::string& folder_name);

        const std::string& getName() const {
            return name_;
        }

        operator std::string& () {
            return name_;
        }

        operator std::string() const {
            return name_;
        }

    private:
        std::string name_;
    };

    /**
     * \brief PLATEAUの3D都市モデルデータ製品へのアクセスを提供します。
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
        std::vector<GmlFileInfo> getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package) override;

        /**
         * \brief 座標範囲で都市モデルデータをフィルタリングします。
         * \param extent 座標範囲
         * \return フィルタリングされた都市モデルデータ
         */
        std::shared_ptr<LocalDatasetAccessor> filter(const geometry::Extent& extent) const;

        /**
         * \brief 座標範囲で都市モデルデータをフィルタリングします。
         * \param extent 座標範囲
         * \param collection フィルタリングされた都市モデルデータの格納先
         */
        void filter(const geometry::Extent& extent, LocalDatasetAccessor& collection) const;

        /**
         * \brief メッシュコードで都市モデルデータをフィルタリングします。
         * \param mesh_codes 欲しい地域IDのvector
         * \param collection フィルタリングされた都市モデルデータの格納先
         */
        void filterByMeshCodes(const std::vector<MeshCode>& mesh_codes, LocalDatasetAccessor& collection) const;

        /**
         * \brief 上の filterByMeshCodes 関数について、shared_ptr で返す版です。
         */
        std::shared_ptr<LocalDatasetAccessor> filterByMeshCodes(const std::vector<MeshCode>& mesh_codes) const;
        
        /**
         * \brief 存在する都市モデルパッケージをマスクとして取得します。
         */
        PredefinedCityModelPackage getPackages();

        /**
         * \brief packageに該当するCityGMLファイルを取得します。
         * \param package 都市モデルパッケージ
         * \param gml_files 取得結果の格納先
         */
        const std::string& getGmlFilePath(PredefinedCityModelPackage package, int index);
        const GmlFileInfo& getGmlFileInfo(PredefinedCityModelPackage package, int index);

        /**
         * \brief packageに該当するCityGMLファイルを取得します。
         *        なければ空のvectorを返します。
         * \param package 都市モデルパッケージ
         * \param gml_files 取得結果の格納先
         */
        std::shared_ptr<std::vector<std::string>> getGmlFiles(PredefinedCityModelPackage package);

        /**
         * \brief packageに該当するCityGMLファイルの個数を取得します。
         * \param package 都市モデルパッケージ
         */
        int getGmlFileCount(PredefinedCityModelPackage package);

        /**
         * \brief 都市モデルデータが存在する地域メッシュのリストを取得します。
         */
        std::vector<MeshCode> getMeshCodes() override;

        std::string getRelativePath(const std::string& path) const;
        std::string getU8RelativePath(const std::string& path) const;

        /**
         * 各メッシュコードの中心地点の平均を求め、直交座標系で返します。
         */
        TVec3d calculateCenterPoint(const plateau::geometry::GeoReference& geo_reference);

    private:
        std::string udx_path_;
        std::map<PredefinedCityModelPackage, std::vector<GmlFileInfo>> files_;
        std::set<MeshCode> mesh_codes_;
        void addFile(PredefinedCityModelPackage sub_folder, const GmlFileInfo& gml_file_info);
        void setUdxPath(std::string udx_path);
    };
}
