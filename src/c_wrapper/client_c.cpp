#include "libplateau_c.h"
#include <plateau/network/client.h>

extern "C" {
using namespace plateau::network;
using namespace libplateau;

DLL_3_ARG_FUNC(plateau_create_client,
                const Client** const out_client_ptr,
                const char* const server_url,
                const char* const api_token,
                *out_client_ptr = new Client(server_url, api_token))

DLL_1_ARG_FUNC(plateau_create_client_for_mock_server,
               const Client** const out_client_ptr,
               *out_client_ptr = new Client(Client::createClientForMockServer()))

DLL_DELETE_FUNC(plateau_delete_client,
                Client)

DLL_2_ARG_FUNC(plateau_client_get_metadata,
               const Client* const client,
               std::vector<DatasetMetadataGroup>* const out_vector_dataset_metadata_group_ptr,
               client->getMetadata(*out_vector_dataset_metadata_group_ptr))

DLL_2_ARG_FUNC(plateau_client_set_api_server_url,
               Client* const client,
               const char* const api_server_url,
               client->setApiServerUrl(std::string(api_server_url)))

DLL_STRING_VALUE_FUNC(plateau_client_get_api_server_url,
                      Client,
                      handle->getApiServerUrl())

DLL_4_ARG_FUNC(plateau_client_download,
               const Client* const client,
               const char* const destination_directory,
               const char* const url,
               std::string* const out_downloaded_path,
               *out_downloaded_path = client->download(destination_directory, url) )


DLL_1_ARG_FUNC(plateau_client_get_mock_server_url,
               std::string* out_mock_server_url,
               *out_mock_server_url = Client::getMockServerUrl())
}
