#pragma once

#include <libplateau_api.h>
#include <plateau/udx/mesh_code.h>

namespace plateau::udx {

    /**
     * \brief GMLファイルに関する情報を保持するクラスです。
     */
    class LIBPLATEAU_EXPORT GmlFile {
    public:
        explicit GmlFile(const std::string& path);

        const std::string& getPath() const;
        void setPath(const std::string& path);
        MeshCode getMeshCode() const;
        const std::string& getFeatureType() const;
        std::string getAppearanceDirectoryPath() const;
        bool isValid() const;

        /**
         * \brief CityGMLファイルとその関連ファイル(テクスチャ、コードリスト)をコピーします。コピー先にすでにファイルが存在する場合はスキップします。
         * \param destination_root_path コピー先のフォルダへのパス。このパスの配下に3D都市モデルデータ製品のルートフォルダが配置されます。
         * \returns コピー先されたCityGMLファイルの情報を返します。
         */
        std::shared_ptr<GmlFile> fetch(const std::string& destination_root_path) const;

        /**
         * \brief CityGMLファイルとその関連ファイル(テクスチャ、コードリスト)をコピーします。コピー先にすでにファイルが存在する場合はスキップします。
         * \param destination_root_path コピー先のフォルダへのパス。このパスの配下に3D都市モデルデータ製品のルートフォルダが配置されます。
         * \param copied_gml_file コピーされたCityGMLファイル
         */
        void fetch(const std::string& destination_root_path, GmlFile& copied_gml_file) const;

    private:
        std::string path_;
        std::string code_;
        std::string feature_type_;
        bool is_valid_;

        void applyPath();
    };
}
