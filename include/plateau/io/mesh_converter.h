#pragma once
#include <string>
#include <fstream>

#include <citygml/citygml.h>
#include <libplateau_api.h>
#include <plateau_dll_logger.h>
#include <plateau/io/mesh_convert_options.h>

class LIBPLATEAU_EXPORT MeshConverter {
public:
    MeshConverter() = default;

    /**
     * \brief GMLファイルをメッシュファイルに変換します。
     *
     * city_modelを引数に指定する場合以下が必要です。
     * - tesselateオプションがtrueでパースされていること
     * - gml_file_pathのGMLファイルがパースされた都市モデルであること
     *
     * \param destination_directory 出力先のディレクトリ
     * \param gml_file_path 入力GMLファイル
     * \param city_model 入力GMLファイルをパースした都市モデル。nullptrを入力した場合は内部でパースされます。
     */
    void convert(const std::string& destination_directory, const std::string& gml_file_path, std::shared_ptr<const citygml::CityModel> city_model = nullptr, std::shared_ptr<PlateauDllLogger> logger = nullptr) const;

    /**
     * \brief メッシュ変換オプションを取得します。
     * \return コピーされたメッシュ変換オプション
     */
    [[nodiscard]] MeshConvertOptions getOptions() const;

    /**
     * \brief メッシュ変換オプションを設定します。
     * \param options メッシュ変換オプション
     */
    void setOptions(const MeshConvertOptions& options);

private:
    MeshConvertOptions options_;
};
