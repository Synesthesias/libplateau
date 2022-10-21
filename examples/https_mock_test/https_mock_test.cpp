#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <iostream>
#include "../../3rdparty/cpp-httplib/httplib.h"
#include "../../3rdparty/json/single_include/nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

int main(void) {
    cout << "c++ Json client start !!" << endl;

    httplib::Client cli("https://9tkm2n.deta.dev");
    auto res = cli.Get("/api/sdk/files?codes=53392642,53392670");

    if (res && res->status == 200) {
        cout << "Response:" << endl;
        auto jres = json::parse(res->body);
        for (json::iterator it = jres.begin(); it != jres.end(); ++it) {
            cout << it.key() << " : " << it.value() << endl;
        }
    }
}