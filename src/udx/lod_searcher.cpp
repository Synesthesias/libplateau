#include <plateau/udx/lod_searcher.h>
#include <fstream>
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
                // ":lod" の次の文字の場所を指すポインタであり、LODの番号を指すことを期待します。
                auto lod_num_ptr = std::min(
                        pos_ptr + lod_mark_size,
                        chunk + read_size - 1
                );

                // ":lod" の直後の数字を求めます。 数字は1桁であることが前提です。
                int lod_num = *lod_num_ptr - u8'0';
                if(0 <= lod_num && lod_num <= 9){
                    lod_flag.setFlag(lod_num);
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
