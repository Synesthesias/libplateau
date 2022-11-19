#include <plateau/network/client.h>
#include <iostream>
#include <filesystem>
#include "../../3rdparty/cpp-httplib/httplib.h"
#include "../../3rdparty/json/single_include/nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace plateau::network {

    std::string Client::getApiServerUrl() {
        return server_url_;
    }

    void Client::setApiServerUrl(const std::string& url) {
        server_url_ = url;
    }

    std::vector<DatasetMetadataGroup> Client::getMetadata() {
        httplib::Client cli(server_url_);
        cli.enable_server_certificate_verification(false);
        auto res = cli.Get("/api/sdk/data");

        std::vector<DatasetMetadataGroup> dataset_meta_data_group_vec;

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
                dataset_meta_data_group_vec.push_back(dataset_meta_data_group);
            }
        }
        return dataset_meta_data_group_vec;
    }

    
    std::shared_ptr<std::vector<plateau::udx::MeshCode>> Client::getMeshCodes(const std::string& id) {
        auto mesh_code_ptr = std::make_shared<std::vector<plateau::udx::MeshCode>>();
        
        httplib::Client cli(server_url_);
        cli.enable_server_certificate_verification(false);
        auto res = cli.Get("/api/sdk/codes/"+id);

        if (res && res->status == 200) {
            auto jres = json::parse(res->body);

            for (int i = 0; i < jres["codes"].size(); i++) {
                std::string code = jres["codes"][i];
                plateau::udx::MeshCode mesh_code(code);
                mesh_code_ptr->push_back(mesh_code);
            }
        }
        return mesh_code_ptr;
    }
    
    std::shared_ptr<std::map<std::string, std::vector<std::string>>> Client::getFiles(const std::vector<plateau::udx::MeshCode>& mesh_codes) {
        auto file_urls = std::make_shared<std::map<std::string, std::vector<std::string>>>();
        
        httplib::Client cli(server_url_);
        cli.enable_server_certificate_verification(false);
        std::string code_str = mesh_codes[0].get();
        for(int i = 1; i < mesh_codes.size(); i++) code_str = code_str + "," + mesh_codes[i].get();
        auto res = cli.Get("/api/sdk/files?codes=" + code_str);

        if (res && res->status == 200) {
            auto jres = json::parse(res->body);
            
            for (json::iterator it = jres.begin(); it != jres.end(); ++it) {
                auto key = it.key();
                if (key == "input")continue;
                auto val = it.value();
                file_urls->emplace(key, val);
            }
        }
        return file_urls;
    }
    
    std::string Client::download(const std::string& destination_directory, const std::string& url) {
        auto gml_file_name = fs::u8path(url).filename().string();
        auto gml_file_path = fs::absolute(fs::u8path(destination_directory) / gml_file_name).u8string();
        auto ofs = std::ofstream(gml_file_path);
        if (!ofs.is_open()) {
            throw std::string("Failed to open stream of gml path : ") + gml_file_path;
        }
        
        httplib::Client cli(server_url_);
        cli.enable_server_certificate_verification(false);
        auto res = cli.Get(url.substr(url.substr(8).find("/") + 8));
        if (res && res->status == 200) {
            ofs << res->body;
        }
        return gml_file_path;
    }
    
}