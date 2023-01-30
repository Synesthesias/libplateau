#include <plateau/network/client.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../../3rdparty/cpp-httplib/httplib.h"
#include "../../3rdparty/json/single_include/nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace plateau::network {
    Client::Client(const std::string& server_url) {
        if (server_url.empty())
            setApiServerUrl(getDefaultServerUrl());
        else
            setApiServerUrl(server_url);
    }

    std::string Client::getApiServerUrl() const {
        return server_url_;
    }

    void Client::setApiServerUrl(const std::string& url) {
        server_url_ = url;
    }

    void Client::getMetadata(std::vector<DatasetMetadataGroup>& out_metadata_groups) const {
        httplib::Client cli(server_url_);
        cli.enable_server_certificate_verification(false);
        auto res = cli.Get(endPointUrlForMetadataGroups());

        if (res && res->status == 200) {
            auto jres = json::parse(res->body);

            for (int i = 0; i < jres["data"].size(); i++) {
                DatasetMetadataGroup dataset_meta_data_group;
                dataset_meta_data_group.id = jres["data"][i]["id"];
                dataset_meta_data_group.title = jres["data"][i]["title"];

                for (int j = 0; j < jres["data"][i]["data"].size(); j++) {
                    DatasetMetadata dataset_meta_data;
                    dataset_meta_data.id = jres["data"][i]["data"][j]["id"];
                    dataset_meta_data.title = jres["data"][i]["data"][j]["title"];
                    dataset_meta_data.description = jres["data"][i]["data"][j]["description"];
                    dataset_meta_data.max_lod = jres["data"][i]["data"][j]["maxLod"];
                    if (jres["data"][i]["data"][j].contains("featureTypes")) {
                        dataset_meta_data.feature_types = jres["data"][i]["data"][j]["featureTypes"];
                    }
                    dataset_meta_data_group.datasets.push_back(dataset_meta_data);
                }
                out_metadata_groups.push_back(dataset_meta_data_group);
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

        httplib::Client cli(server_url_);
        cli.enable_server_certificate_verification(false);
        auto res = cli.Get(endPointUrlForFiles(id));

        DatasetFiles dataset_files;

        if (res && res->status == 200) {
            auto jres = json::parse(res->body);
            for (json::iterator it = jres.begin(); it != jres.end(); ++it) {
                auto& key = it.key(); // パッケージ種です。例: "bldg"
                auto& file_items = it.value(); // そのパッケージ種に属する、任意個数のファイル情報です。

                dataset_files.emplace(key, std::vector<DatasetFileItem>());
                for (const auto& file_item : file_items) { // 各ファイルについて
                    DatasetFileItem dataset_file;
                    dataset_file.max_lod = file_item["maxLod"].get<int>();
                    dataset_file.mesh_code = file_item["code"];
                    dataset_file.url = file_item["url"];

                    dataset_files[key].push_back(dataset_file);
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

        httplib::Client cli(server_url_);
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

    const std::string& Client::getDefaultServerUrl() {
        static const std::string default_server_url = "https://plateau-api-mock-v2.deta.dev";
        return default_server_url;
    }
}
