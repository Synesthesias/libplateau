#include "libplateau_c.h"
#include <plateau/network/network_config.h>

using namespace plateau::network;
extern "C" {

DLL_1_ARG_FUNC(plateau_network_config_mock_server_url,
               std::string*,
               *arg = NetworkConfig::mockServerUrl())
}
