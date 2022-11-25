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
}
