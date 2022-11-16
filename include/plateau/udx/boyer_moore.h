#pragma once
#include <libplateau_api.h>
#include <string>
#include <map>

namespace plateau::udx{
    // 参考 : https://www.yasuhisay.info/entry/20091215/1260835159
    class LIBPLATEAU_EXPORT BoyerMoore {
    public:
        std::string pattern_;
        int m_;
        std::map<char, int> lambda_;
        explicit BoyerMoore(const std::string& pattern);
        void computeLambda();
        int getLambda(const char& c) const;
        char* match(char* search_start_ptr, char* search_end_ptr) const;
    };

}
