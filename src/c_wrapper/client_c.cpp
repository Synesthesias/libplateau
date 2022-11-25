#include "libplateau_c.h"
#include <plateau/network/client.h>

extern "C"{
    using namespace plateau::network;

DLL_CREATE_FUNC(plateau_create_client,
                Client)

DLL_DELETE_FUNC(plateau_delete_client,
                Client)

DLL_2_ARG_FUNC(plateau_client_get_metadata,
               const Client* const,
               std::vector<DatasetMetadataGroup>* const,
               arg_1->getMetadata(*arg_2))

DLL_2_ARG_FUNC(plateau_set_api_server_url,
              Client* const,
              const char* const,
              arg_1->setApiServerUrl(std::string(arg_2)))

DLL_STRING_VALUE_FUNC(plateau_get_api_server_url,
                      Client,
                      handle->getApiServerUrl())
}
