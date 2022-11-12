#include <plateau/udx/available_lod_searcher.h>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace plateau::udx;
namespace fs = std::filesystem;

namespace{
    LodFlag searchLodsInStr(const std::string& str){
        // 文字列検索で ":lod" にヒットした直後の数値をLODとします。
        const static auto lod_mark = std::string(u8":lod");
        const static auto lod_mark_size = lod_mark.size();
        // ":lod" の直後の数値を取得するための正規表現です。
        const static auto reg_lod_num = std::regex(R"(\d+)");
        // 最大LOD値の10進数での桁数であり、正規表現で数値を検索する範囲に影響します。
        const static auto max_lod_digits_in_decimal = std::to_string(LodFlag::max_lod_).length();

        const auto str_size = str.size();
        // 検索を始めます。
        auto lod_flag = LodFlag();
        auto pos = str.find(lod_mark);

        while(pos != std::string::npos){
            // ":lod" の次の文字の場所
            auto reg_search_begin_pos = std::min(
                    pos + lod_mark_size,
                    str_size
                    );
            // ":lod" の次から数字の桁数だけ正規表現で検索、その範囲の終了場所
            auto reg_search_end_pos = std::min(
                    reg_search_begin_pos + max_lod_digits_in_decimal + 1,
                    str_size
                    );
            auto reg_search_begin_iter = str.begin() + (long long)reg_search_begin_pos;
            auto reg_search_end_iter = str.begin() + (long long)reg_search_end_pos;
            auto reg_results = std::smatch();
            if(std::regex_search(reg_search_begin_iter, reg_search_end_iter, reg_results, reg_lod_num)){
                unsigned lod = (unsigned)std::stoi(reg_results.str());
                if(lod <= LodFlag::max_lod_){
                    lod_flag.setFlag(lod);
                }
            }
            pos = str.find(lod_mark, pos + lod_mark_size);
        }
        return lod_flag;
    }
}

LodFlag AvailableLodSearcher::searchLodsInFile(const fs::path& file_path) {
    auto ifs = std::ifstream (file_path);
    if(!ifs){
        throw std::runtime_error("Failed to read file.");
    }
    std::ostringstream buffer;
    buffer << ifs.rdbuf();
    std::string file_body = buffer.str();
    return searchLodsInStr(file_body);
}



namespace{
    void throwIfOutOfRange(unsigned digit){
        if(digit > LodFlag::max_lod_){
            throw std::range_error("argument digit is out of range.");
        }
    }
}

void LodFlag::setFlag(unsigned digit) {
    throwIfOutOfRange(digit);
    flags_ |= (1 << digit);
}

void LodFlag::unsetFlag(unsigned digit) {
    throwIfOutOfRange(digit);
    flags_ &= ~(1 << digit);
}

unsigned LodFlag::getFlag() const{
    return flags_;
}
