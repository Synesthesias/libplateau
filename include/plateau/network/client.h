#pragma once
#include <string>
#include <vector>
#include <plateau/dataset/mesh_code.h>

namespace plateau::network {

    /**
     * データセットの情報です。
     */
    struct DatasetMetadata {
        std::string id;
        std::string title;
        std::string description;
        std::vector<std::string> feature_types;
    };

    /**
     * データセットに含まれるファイルです。
     */
    struct DatasetFileItem {
        std::string mesh_code;
        std::string url;
        int max_lod = 0;
    };

    /**
     * データセットに含まれるファイルの一覧です。
     */
    typedef std::map<std::string, std::vector<DatasetFileItem>> DatasetFiles;

    /**
     * データセットグループの一覧です。
     * 通常は都道府県の一覧となります。
     * 各データセットグループ（都道府県）の下にデータセットの一覧があります。
     */
    struct DatasetMetadataGroup {
        std::string id;
        std::string title;
        std::vector<DatasetMetadata> datasets;
    };

    /**
     * PLATEAUのAPIサーバーへ接続し、REST APIを介して通信するために使用されます。
     */
    class LIBPLATEAU_EXPORT Client {
    public:
        Client(const std::string& server_url, const std::string& api_token);
        static Client createClientForMockServer();

        std::string getApiServerUrl() const;
        void setApiServerUrl(const std::string& url);
        void setApiToken(const std::string& api_token);
        std::shared_ptr<std::vector<DatasetMetadataGroup>> getMetadata() const;
        void getMetadata(std::vector<DatasetMetadataGroup>& out_metadata_groups) const;

        /**
         * @brief サーバーから json を受け取り、それをパースしてデータファイルに関する情報を得ます。
         * 受け取る json の例 : https://plateau-api-mock-v2.deta.dev/api/sdk/datasets/23ku/files
         */
        DatasetFiles getFiles(const std::string& id) const;
        std::string download(const std::string& destination_directory_path, const std::string& url) const;

        static const std::string& getDefaultServerUrl();
        static const std::string& getMockServerUrl();
        static const std::string& getDefaultApiToken(); // Bearer認証で使います。
    private:
        std::string server_url_;
        std::string api_token_;

        static std::string endPointUrlForMetadataGroups() { return "/api/sdk/datasets"; }
        static std::string endPointUrlForFiles(const std::string& id) { return "/api/sdk/datasets/" + id + "/files"; }
    };
}
