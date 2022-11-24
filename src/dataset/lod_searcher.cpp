#include <plateau/dataset/lod_searcher.h>
#include <fstream>
#include <stdexcept>
#include <cstring>

using namespace plateau::udx;
namespace fs = std::filesystem;

namespace {
    LodFlag searchLods(std::ifstream& ifs) {
        // 注意:
        // この関数は実行速度にこだわる必要があります。
        // 用途はPLATEAUデータのインポートにおける範囲選択画面で、多くのGMLファイルについて利用可能なLODを検索します。

        // 文字列検索で ":lod" にヒットした直後の数値をLODとします。
        const static auto lod_pattern = u8":lod";
        const static auto lod_pattern_size = strlen(lod_pattern);

        auto lod_flag = LodFlag();

        // GMLファイルを全部メモリに読み込むと重いので、16KBごとに分割して読み込みます。
        // 分割された1つを チャンク(chunk) と名付けます。
        constexpr int chunk_size = 16 * 1024;
        char chunk[chunk_size];
        const char* const chunk_const = chunk;

        ifs.read(chunk, sizeof chunk);
        auto read_size = ifs.gcount();
        while (!ifs.eof()) {
            const char* const chunk_last = chunk_const + read_size - 1; // チャンク内の最後の文字のポインタ
            // チャンク内での文字列検索を始めます。 ":lod" を探します。
            const char* found_ptr = strstr(chunk, lod_pattern);

            // ":lod" がヒットするたびに、その直後の数字をLODとみなして取得します。
            while (found_ptr) {
                // ":lod" の次の文字を指すポインタです。そこはLODの番号を指すことを期待します。
                auto lod_num_ptr = std::min(
                        found_ptr + lod_pattern_size,
                        chunk_last
                );

                // ":lod" の直後の数字を求めます。 数字は1桁であることが前提です。
                int lod_num = *lod_num_ptr - u8'0';
                if (0 <= lod_num && lod_num <= 9) {
                    lod_flag.setFlag(lod_num);
                }

                // チャンク内の次の ":lod" を探します。
                auto next_pos = std::min(found_ptr + lod_pattern_size, chunk_last);
                found_ptr = strstr(next_pos, lod_pattern);
            }
            // 次のチャンクに移ります。
            ifs.read(chunk, sizeof chunk);
            read_size = ifs.gcount();
        }


        return lod_flag;
    }
}

LodFlag LodSearcher::searchLodsInFile(const fs::path& file_path) {
    auto ifs = std::ifstream(file_path);
    if (!ifs) {
        throw std::runtime_error("Failed to read file.");
    }
    return searchLods(ifs);
}


namespace {
    void throwIfOutOfRange(unsigned digit) {
        if (digit > LodFlag::max_lod_) {
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

int LodFlag::getMax() const {
    auto flag = flags_;
    int i = -1;
    while(flag > 0){
        ++i;
        flag >>= 1;
    }
    return i;
}

unsigned LodFlag::getFlag() const {
    return flags_;
}
