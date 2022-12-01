#include "libplateau_c.h"
#include <plateau/network/client.h>

extern "C" {
using namespace plateau::network;
using namespace libplateau;

DLL_CREATE_FUNC(plateau_create_client,
                Client)

DLL_DELETE_FUNC(plateau_delete_client,
                Client)

DLL_2_ARG_FUNC(plateau_client_get_metadata,
               const Client* const,
               std::vector<DatasetMetadataGroup>* const,
               arg_1->getMetadata(*arg_2))

DLL_2_ARG_FUNC(plateau_client_set_api_server_url,
               Client* const,
               const char* const,
               arg_1->setApiServerUrl(std::string(arg_2)))

DLL_STRING_VALUE_FUNC(plateau_client_get_api_server_url,
                      Client,
                      handle->getApiServerUrl())

LIBPLATEAU_C_EXPORT libplateau::APIResult LIBPLATEAU_C_API plateau_client_download(
        const Client* const client,
        const char* const destination_directory_utf8,
        const char* const url_utf8,
        std::string* const out_downloaded_path
) {
    API_TRY {
        *out_downloaded_path = client->download(destination_directory_utf8, url_utf8);
        return APIResult::Success;
    } API_CATCH
    return APIResult::ErrorUnknown;
}
}
