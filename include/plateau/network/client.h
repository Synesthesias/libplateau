#pragma once
#include <string>
#include <vector>
#include <plateau/dataset/mesh_code.h>

namespace plateau::network {

    struct DatasetMetadata {
        std::string id;
        std::string title;
        std::string description;
        int max_lod;
        std::vector<std::string> feature_types;
    };
    
    struct DatasetMetadataGroup {
        std::string id;
        std::string title;
        std::vector<DatasetMetadata> datasets;
    };

    /**
     * サーバー上にあるPLATEAU都市データについて、
     * どのようなデータがあるのかをクエリで問い合わせます。
     * またファイルをダウンロードします。
     *
     * 処理の流れ:
     * ・ getMetadata でデータセットの一覧を取得し、そのうちの1つをユーザーが選択します。例: 東京23区、横浜市
     * ・ 選択したデータセットのIDについて getMeshCodes で地域メッシュコードIDの一覧を取得し、それをもとにユーザーが範囲選択します。
     * ・ 選択した範囲について、 getFiles でファイルの一覧を取得します。
     * ・ ファイルを download します。
     */
    class LIBPLATEAU_EXPORT Client {
    public:
        Client() : server_url_("") {
        }

        std::string getApiServerUrl();
        void setApiServerUrl(const std::string& url);
        std::vector<DatasetMetadataGroup> getMetadata();
        std::shared_ptr<std::vector<plateau::dataset::MeshCode>> getMeshCodes(const std::string& id);
        std::shared_ptr<std::map<std::string, std::vector<std::string>>> getFiles(const std::vector<plateau::dataset::MeshCode>& mesh_codes);
        std::string download(const std::string& destination_directory, const std::string& url);
    private:
        std::string server_url_;
    };
}
