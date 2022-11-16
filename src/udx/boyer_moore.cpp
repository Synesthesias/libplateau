#include <plateau/udx/boyer_moore.h>
#include <string>
#include <map>

using namespace plateau::udx;

BoyerMoore::BoyerMoore(const std::string& pattern) :
        pattern_(pattern), m_(pattern.size()) {
    computeLambda();
}

void BoyerMoore::computeLambda() {
    for (int j = 1; j <= m_; j++) {
        lambda_[pattern_.at(j - 1)] = j;
    }
}

int BoyerMoore::getLambda(const char& c) const {
    if (lambda_.find(c) != lambda_.end()) {
        return lambda_.at(c);
    } else {
        return 0;
    }
}

char* BoyerMoore::match(char* search_start_ptr, char* search_end_ptr) const {
    auto s = search_start_ptr;
    while (s <= search_end_ptr - m_) {
        int j = m_;
        while (j > 0 && pattern_.at(j - 1) == *(s + j - 1)) {
            j--;
        }
        if (j == 0) {
            return s;
        } else {
            s += std::max(1, j - getLambda(*(s + j - 1)));
        }
    }
    return search_end_ptr;


//    auto match_ptr = strstr(search_start_ptr, pattern_.c_str());
//    if(match_ptr == NULL) return search_end_ptr;
//    return match_ptr;
}
