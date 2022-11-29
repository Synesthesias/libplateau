#include <plateau/network/client.h>
#include <iostream>
#include <filesystem>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../../3rdparty/cpp-httplib/httplib.h"
#include "../../3rdparty/json/single_include/nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace plateau::network {

    Client::Client(const std::string& server_url){
        setApiServerUrl(server_url);
    }

    std::string Client::getApiServerUrl() const {
        return server_url_;
    }

    void Client::setApiServerUrl(const std::string& url) {
        server_url_ = url;
    }

    std::vector<DatasetMetadataGroup> Client::getMetadata() const {
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

    void Client::getMetadata(std::vector<DatasetMetadataGroup>& out_metadata_groups) const {
        out_metadata_groups = getMetadata();
    }

    
    std::vector<plateau::dataset::MeshCode> Client::getMeshCodes(const std::string& id) {
        auto ret_mesh_codes = std::vector<plateau::dataset::MeshCode>();
        
        httplib::Client cli(server_url_);
        cli.enable_server_certificate_verification(false);
        auto res = cli.Get("/api/sdk/codes/"+id);

        if (res && res->status == 200) {
            auto jres = json::parse(res->body);

            for (int i = 0; i < jres["codes"].size(); i++) {
                std::string code = jres["codes"][i];
                plateau::dataset::MeshCode mesh_code(code);
                ret_mesh_codes.push_back(mesh_code);
            }
        }
        return ret_mesh_codes;
    }
    
    std::shared_ptr<std::map<std::string, std::vector<std::pair<float, std::string>>>> Client::getFiles(const std::vector<plateau::dataset::MeshCode>& mesh_codes) {
        auto file_url_lod = std::make_shared<std::map<std::string, std::vector<std::pair<float, std::string>>>>();
             
        httplib::Client cli(server_url_);
        cli.enable_server_certificate_verification(false);
        std::string code_str = mesh_codes[0].get();
        for(int i = 1; i < mesh_codes.size(); i++) code_str = code_str + "," + mesh_codes[i].get();
        auto res = cli.Get("/api/sdk/files?codes=" + code_str);

        if (res && res->status == 200) {
            auto jres = json::parse(res->body);
            for (json::iterator it = jres.begin(); it != jres.end(); ++it) {
                auto key = it.key();
                if (key == "codes") continue;
                auto vec_dic = it.value();
                std::vector<std::pair<float, std::string>> vp;
                for (int i = 0; i < vec_dic.size(); i++) {
                    std::pair<float, std::string> p = std::make_pair(std::stof((std::string)vec_dic[i]["maxLod"]), vec_dic[i]["url"]);
                    vp.push_back(p);
                }
                file_url_lod->emplace(key, vp);
            }
        }
        return file_url_lod;
    }
    
    std::string Client::download(const std::string& destination_directory_utf8, const std::string& url_utf8) const {
        auto gml_file_name = fs::u8path(url_utf8).filename().string();
        auto gml_file_path = fs::absolute(fs::u8path(destination_directory_utf8) / gml_file_name).u8string();
        auto ofs = std::ofstream(gml_file_path);
        if (!ofs.is_open()) {
            throw std::logic_error(std::string("Failed to open stream of gml path : ") + gml_file_path);
        }
        
        httplib::Client cli(server_url_);
        cli.enable_server_certificate_verification(false);
        auto path = url_utf8.substr(url_utf8.substr(8).find("/") + 8);
        auto res = cli.Get(path);
        if (res && res->status == 200) {
            ofs << res->body;
        }
        return gml_file_path;
    }
    
}
