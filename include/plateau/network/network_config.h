#pragma once
#include <string>
namespace plateau::network{
    class NetworkConfig{
    public:
        static std::string mockServerUrl(){
            return "https://plateau-api-mock.deta.dev";
        }
    };
}
