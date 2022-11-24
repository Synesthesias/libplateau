#include <plateau/network/client.h>

int main(void) {
    plateau::network::Client client;

    client.setApiServerUrl("https://9tkm2n.deta.dev");

    // check Client::getMetadata
    auto meta_data = client.getMetadata();
    for (int i = 0; i < meta_data.size(); i++) {
        std::cout << "data : " << i << std::endl;
        std::cout << "id : " << meta_data[i].id << std::endl;
        std::cout << "title : " << meta_data[i].title << std::endl;
        for (int j = 0; j < meta_data[i].datasets.size(); j++) {
            std::cout << "  data : " << j << std::endl;
            std::cout << "  id : " << meta_data[i].datasets[j].id << std::endl;
            std::cout << "  title : " << meta_data[i].datasets[j].title << std::endl;
            std::cout << "  description : " << meta_data[i].datasets[j].description << std::endl;
            std::cout << "  max_lod : " << meta_data[i].datasets[j].max_lod << std::endl;
            std::cout << "  feature_types : ";
            for(int k = 0; k < meta_data[i].datasets[j].feature_types.size(); k++) std::cout << meta_data[i].datasets[j].feature_types[k] << " ,";
            std::cout << std::endl;
        }
    }

    // check Client::getMeshCodes
    auto get_mesh_codes = client.getMeshCodes("23ku");
    std::cout << "mesh codes : ";
    for(int i = 0; i<get_mesh_codes->size(); i++) std::cout << get_mesh_codes->at(i).get() << ", ";
    std::cout << std::endl;

    // check Client::getFiles
    std::vector<plateau::dataset::MeshCode> set_mesh_codes;
    set_mesh_codes.push_back(plateau::dataset::MeshCode("53392642"));
    set_mesh_codes.push_back(plateau::dataset::MeshCode("53392670"));
    auto file_urls = client.getFiles(set_mesh_codes);
    for (auto itr = file_urls->begin(); itr != file_urls->end(); ++itr) {
        std::cout << itr->first << " : ";
        for (int j = 0; j < itr->second.size(); j++) std::cout << itr->second[j] << ", ";
        std::cout << std::endl;
    }

    // check Client::download
    auto fpath = client.download(".", "https://9tkm2n.deta.dev/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml");
    std::cout << "gml @ " << fpath << std::endl;
    
}
