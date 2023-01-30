#include <plateau/network/client.h>

int main(void) {
    plateau::network::Client client;

    client.setApiServerUrl(plateau::network::Client::getDefaultServerUrl());

    // check Client::getMetadata
    auto meta_data_ptr = client.getMetadata();
    auto meta_data = *meta_data_ptr;
    for (int i = 0; i < meta_data.size(); i++) {
        std::cout << "data : " << i << std::endl;
        std::cout << "id : " << meta_data[i].id << std::endl;
        std::cout << "title : " << meta_data[i].title << std::endl;
        for (int j = 0; j < meta_data[i].datasets.size(); j++) {
            std::cout << "  data : " << j << std::endl;
            std::cout << "  id : " << meta_data[i].datasets[j].id << std::endl;
            std::cout << "  title : " << meta_data[i].datasets[j].title << std::endl;
            std::cout << "  description : " << meta_data[i].datasets[j].description << std::endl;
            std::cout << "  feature_types : ";
            for(int k = 0; k < meta_data[i].datasets[j].feature_types.size(); k++) std::cout << meta_data[i].datasets[j].feature_types[k] << " ,";
            std::cout << std::endl;
        }
    }

    // check Client::download
    auto fpath = client.download(".", plateau::network::Client::getDefaultServerUrl() + "/13100_tokyo23-ku_2020_citygml_3_2_op/udx/bldg/53392642_bldg_6697_2_op.gml");
    std::cout << "gml @ " << fpath << std::endl;
}
