#include <plateau/udx/boyer_moore.h>
#include <string>
#include <map>

using namespace plateau::udx;
    BoyerMoore::BoyerMoore(/*std::string text_,*/ const std::string& pattern_) :
/*text(text_), */pattern(pattern_), /*n(text_.size()),*/ m(pattern_.size()){
    compute_lambda();
}

    void BoyerMoore::compute_lambda() {
        for(int j = 1; j <= m; j++) {
            lambda[pattern.at(j - 1)] = j;
        }
    }

    int BoyerMoore::get_lambda(const char& c) {
        if (lambda.find(c) != lambda.end()) {
            return lambda[c];
        } else {
            return 0;
        }
    }

    char* BoyerMoore::match(char* search_start_ptr, char* search_end_ptr) {
        auto s = search_start_ptr;
        while(s <= search_end_ptr - m) {
            int j = m;
            while(j > 0 && pattern.at(j - 1) == *(s + j - 1)) {
                j--;
            }
            if(j == 0) {
                return s;
            } else {
                s += std::max(1, j - get_lambda(*(s + j - 1)));
            }
        }
        return search_end_ptr;
    }
