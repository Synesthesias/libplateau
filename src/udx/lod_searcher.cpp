#include <plateau/udx/lod_searcher.h>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <sstream>
#include <iostream>

using namespace plateau::udx;
namespace fs = std::filesystem;

namespace{
    LodFlag searchLods(std::ifstream& ifs){
        // 文字列検索で ":lod" にヒットした直後の数値をLODとします。
        const static auto lod_mark = std::string(u8":lod");
        const static auto lod_mark_size = lod_mark.size();
        // ":lod" の直後の数値を取得するための正規表現です。
        const static auto reg_lod_num = std::regex(R"(\d+)");
        // 最大LOD値の10進数での桁数であり、正規表現で数値を検索する範囲に影響します。
        const static auto max_lod_digits_in_decimal = std::to_string(LodFlag::max_lod_).length();


        auto lod_flag = LodFlag();

        // GMLファイルを全部メモリに読み込むと重いので、16KBごとに分割して読み込みます。
        // 分割された1つを チャンク(chunk) と名付けます。
        constexpr int chunk_size = 16 * 1024;
        char chunk[chunk_size];

        ifs.read(chunk, sizeof chunk);
        auto read_size = ifs.gcount();
        while((!ifs.eof()) && read_size > 0){
            // チャンク内での文字列検索を始めます。 ":lod" を探します。
            auto pos_ptr = strstr(chunk, lod_mark.c_str());

            // ":lod" がヒットするたびに、その直後の数字をLODとみなして取得します。
            while(pos_ptr){
                // ":lod" の次の文字の場所で、正規表現検索の開始地点
                auto reg_search_begin_ptr = std::min(
                        pos_ptr + lod_mark_size,
                        chunk + read_size - 1
                );
                // ":lod" の次から数字の桁数だけ正規表現で検索、その範囲の終了場所
                auto reg_search_end_ptr = std::min(
                        reg_search_begin_ptr + max_lod_digits_in_decimal + 1,
                        chunk + read_size - 1
                );
                auto reg_results = std::smatch();
                auto reg_str = std::string(reg_search_begin_ptr, std::distance(reg_search_begin_ptr, reg_search_end_ptr));
                if(std::regex_search(reg_str.cbegin(), reg_str.cend(), reg_results, reg_lod_num)){
                    unsigned lod = (unsigned)std::stoi(reg_results.str());
                    if(lod <= LodFlag::max_lod_){
                        lod_flag.setFlag(lod);
                    }
                }
                // チャンク内の次の ":lod" を探します。
                auto next_pos = std::min(pos_ptr + lod_mark_size, chunk + chunk_size - 1);
                pos_ptr = strstr(next_pos, lod_mark.c_str());
            }
            // 次のチャンクに移ります。
            ifs.read(chunk, sizeof chunk);
            read_size = ifs.gcount();
        }


        return lod_flag;
    }
}

LodFlag LodSearcher::searchLodsInFile(const fs::path& file_path) {
    auto ifs = std::ifstream (file_path);
    if(!ifs){
        throw std::runtime_error("Failed to read file.");
    }
//    auto buffer = std::ostringstream();
//    buffer << ifs.rdbuf();
//    std::string file_body = buffer.str();
    return searchLods(ifs);
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
