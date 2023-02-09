#include <plateau/network/client.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "../../3rdparty/cpp-httplib/httplib.h"
#include "../../3rdparty/json/single_include/nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace {
    /// httplib::Client を作成し、それにURLとAPIトークンを設定して返します。
    httplib::Client createHttpLibClient(const std::string& url, const std::string& api_token) {
        auto client = httplib::Client(url);
        // Bearer認証を設定します。
        httplib::Headers headers = {{"Authorization", "Bearer " + api_token}};
        client.set_default_headers(headers);
        return client;
    }

    /// 本番APIサーバーのURLをデフォルト値とします。
    const std::string& getDefaultServerUrl() {
        static const std::string default_server_url = "https://api.plateau.reearth.io";
        return default_server_url;
    }

    /**
     * サーバーへの接続にあたって Bearer認証で使うトークンであり、本番サーバーへの接続に必要なものです。
     * なおモックサーバーへの接続時はトークンは空文字で良いです。
     */
    const std::string& getDefaultApiToken() {
        static const std::string default_api_token = "secret-56c66bcac0ab4724b86fc48309fe517a";
        return default_api_token;
    }
}

namespace plateau::network {
    Client::Client(const std::string& server_url, const std::string& api_token) {
        setApiServerUrl(server_url.empty() ? getDefaultServerUrl() : server_url);
        setApiToken(api_token.empty() ? getDefaultApiToken() : api_token);
    }

    Client Client::createClientForMockServer() {
        return Client(Client::getMockServerUrl(), "");
    }

    std::string Client::getApiServerUrl() const {
        return server_url_;
    }

    void Client::setApiServerUrl(const std::string& url) {
        server_url_ = url;
    }

    void Client::setApiToken(const std::string& api_token) {
        api_token_ = api_token;
    }

    void Client::getMetadata(std::vector<DatasetMetadataGroup>& out_metadata_groups) const {
        auto cli = createHttpLibClient(server_url_, api_token_);
        cli.enable_server_certificate_verification(false);
        auto res = cli.Get(endPointUrlForMetadataGroups());

        if (res && res->status == 200) {
            auto jres = json::parse(res->body);

            for (auto & json_dataset_metadata_group : jres.at("data")) {
                DatasetMetadataGroup dataset_metadata_group;
                dataset_metadata_group.id = json_dataset_metadata_group.at("id");
                dataset_metadata_group.title = json_dataset_metadata_group.at("title");

                for (auto & json_dataset_metadata : json_dataset_metadata_group.at("data")) {
                    DatasetMetadata dataset_metadata;
                    dataset_metadata.id = json_dataset_metadata.at("id");
                    dataset_metadata.title = json_dataset_metadata.at("title");
                    dataset_metadata.description = json_dataset_metadata.at("description");
                    if (json_dataset_metadata.contains("featureTypes")) {
                        dataset_metadata.feature_types = json_dataset_metadata.at("featureTypes");
                    }
                    dataset_metadata_group.datasets.push_back(dataset_metadata);
                }
                out_metadata_groups.push_back(dataset_metadata_group);
            }
        }
    }

    std::shared_ptr<std::vector<DatasetMetadataGroup>> Client::getMetadata() const {
        auto result = std::make_shared<std::vector<DatasetMetadataGroup>>();
        getMetadata(*result);
        return result;
    }

    DatasetFiles Client::getFiles(const std::string& id) const {
        auto file_url_lod = std::make_shared<std::map<std::string, std::vector<std::pair<float, std::string>>>>();
        auto cli = createHttpLibClient(server_url_, api_token_);
        cli.enable_server_certificate_verification(false);
        auto res = cli.Get(endPointUrlForFiles(id));

        DatasetFiles dataset_files;

        if (res && res->status == 200) {
            auto jres = json::parse(res->body);
            for (json::iterator it = jres.begin(); it != jres.end(); ++it) {
                auto& key = it.key(); // パッケージ種です。例: "bldg"
                auto& file_items = it.value(); // そのパッケージ種に属する、任意個数のファイル情報です。

                dataset_files.emplace(key, std::vector<DatasetFileItem>());
                for (const auto& file_item: file_items) { // 各ファイルについて
                    DatasetFileItem dataset_file;
                    dataset_file.max_lod = file_item.at("maxLod").get<int>();
                    dataset_file.mesh_code = file_item.at("code");
                    dataset_file.url = file_item.at("url");

                    dataset_files.at(key).push_back(dataset_file);
                }
            }
        }
        return dataset_files;
    }

    std::string Client::download(const std::string& destination_directory_path, const std::string& url_str_arg) const {
        auto url_str = url_str_arg;
        auto destination_directory = fs::u8path(destination_directory_path);
        auto url = fs::u8path(url_str);
        auto gml_file = url.filename();
        auto gml_file_path = fs::absolute(fs::path(destination_directory) / gml_file);
        fs::create_directories(destination_directory);

        auto cli = createHttpLibClient(server_url_, api_token_);
        cli.enable_server_certificate_verification(false);

        // '\\' を '/' に置換
        auto pos = url_str.find(u8'\\');
        while (pos != std::string::npos) {
            url_str.replace(pos, 1, u8"/");
            pos = url_str.find(u8'\\', pos + 1);
        }

        auto path_after_domain = url_str.substr(url_str.substr(8).find('/') + 8);
        auto res = cli.Get(path_after_domain);
        auto content_type = res->get_header_value("Content-Type");
        bool is_text =
                content_type.find("text") != std::string::npos ||
                content_type.find("json") != std::string::npos;
        auto ofs_mode = std::ios_base::openmode(is_text ? 0 : std::ios::binary);
        auto ofs = std::ofstream(gml_file_path.c_str(), ofs_mode);
        if (!ofs.is_open()) {
            throw std::logic_error(std::string("Failed to open stream of gml path : ") + gml_file_path.u8string());
        }
        if (res && res->status == 200) {
            const auto& body = res->body;
            ofs.write(body.c_str(), body.size());
        }
        return gml_file_path.u8string();
    }

    const std::string& Client::getMockServerUrl() {
        static const std::string mock_server_url = "https://plateau-api-mock-v2.deta.dev";
        return mock_server_url;
    }
}
