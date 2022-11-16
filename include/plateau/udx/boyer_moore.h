#pragma once
#include <libplateau_api.h>
#include <string>
#include <map>

namespace plateau::udx{
    // 参考 : https://www.yasuhisay.info/entry/20091215/1260835159
    class LIBPLATEAU_EXPORT BoyerMoore {
    public:
//    std::string text;
        std::string pattern;
//    int n;
        int m;
        std::map<char, int> lambda;
        explicit BoyerMoore(/*std::string text_,*/ const std::string& pattern_);

        void compute_lambda();

        int get_lambda(const char& c);

        char* match(char* search_start_ptr, char* search_end_ptr);
    };

}
