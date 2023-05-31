#include <plateau/dataset/lod_searcher.h>
#include <fstream>
#include <stdexcept>
#include <cstring>

using namespace plateau::dataset;
namespace fs = std::filesystem;

namespace {

    const char* getCharPtr(const char* found_ptr, const size_t lod_pattern_size, size_t offset, const char* const chunk_last){
        return std::min(
                found_ptr + lod_pattern_size + offset,
                chunk_last
        );
    }

    void readCharAndSetLod(const char* found_ptr, const size_t lod_pattern_size, size_t offset, const char* const chunk_last, LodFlag& lod_flag){
        // ":lod" の次の文字 + offset を指すポインタです。そこはLODの番号を指すことを期待します。
        auto lod_num_ptr = getCharPtr(found_ptr, lod_pattern_size, offset, chunk_last);

        // ":lod" の直後の数字を求めます。 数字は1桁であることが前提です。
        int lod_num = *lod_num_ptr - u8'0';
        if (0 <= lod_num && lod_num <= 9) {
            lod_flag.setFlag(lod_num);
        }
    }

}

LodFlag LodSearcher::searchLodsInFile(const fs::path& file_path) {
    auto ifs = std::ifstream(file_path);
    if (!ifs) {
        throw std::runtime_error("Failed to read file.");
    }
    return searchLodsInIstream(ifs);
}

LodFlag LodSearcher::searchLodsInIstream(std::istream& ifs) {
    // 注意:
    // この関数は実行速度にこだわる必要があるため、 std::string よりも原始的な C言語の機能を積極的に利用しています。
    // 用途はPLATEAUデータのインポートにおける範囲選択画面で、GMLファイルについて利用可能なLODを検索します。
    // 範囲選択画面では多くのGMLファイルを検索対象とするので、高速である必要があります。

    // 文字列検索で ":lod" にヒットした直後の数値をLODとします。
    const static auto lod_pattern = u8":lod";
    const static auto lod_pattern_size = strlen(lod_pattern);

    // GMLファイルを全部メモリに読み込むと重いので、16KBごとに分割して読み込みます。
    // 分割された1つを チャンク(chunk) と名付けます。
    // この手法のデメリットとして、チャンクの切れ目で ":lod(num)" が分断された場合には見逃してしまいます。
    // しかし、 ":lod(num)" はGML内で何度も登場することを考えれば、低確率で見逃しても他で検知できる確率が高いのでよしとします。
    constexpr int chunk_read_size = 16 * 1024;
    constexpr int chunk_mem_size = chunk_read_size + 1; // null終端文字の分
    char chunk[chunk_mem_size];
    const char* const chunk_const = chunk;
    auto lod_flag = LodFlag();

    do {
        ifs.read(chunk, chunk_read_size);
        auto read_size = ifs.gcount();
        // ifstream.read() では null終端文字は付きませんが、付けておかないと下の strstr でバグるので追加します。
        chunk[read_size] = '\0';

        const char* const chunk_last = chunk_const + read_size - 1; // チャンク内の最後の文字のポインタ
        // チャンク内での文字列検索を始めます。 ":lod" を探します。
        const char* found_ptr = strstr(chunk, lod_pattern);

        // ":lod" がヒットするたびに、その直後の数字をLODとみなして取得します。
        while (found_ptr) {
            // ":lod"の直後が数字であれば、そのLODをセット
            readCharAndSetLod(found_ptr, lod_pattern_size, 0, chunk_last, lod_flag);
            // ":lod" の直後が ">" であれば、 ":lod>"の直後の数字のLODをセット (dem向け)
            if (*getCharPtr(found_ptr, lod_pattern_size, 0, chunk_last) == u8'>'){
                readCharAndSetLod(found_ptr, lod_pattern_size, 1, chunk_last, lod_flag);
            }

            // チャンク内の次の ":lod" を探します。
            auto next_pos = std::min(found_ptr + lod_pattern_size, chunk_last);
            found_ptr = strstr(next_pos, lod_pattern);
        }
    } while (!ifs.eof());


    return lod_flag;
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

/// 存在するLODのフラグ列のうち、最大のLODを返します。
/// LODが存在しない場合は -1 を返します。
/// LOD i が存在する場合、LOD 0 ～ i-1 も存在することが前提です。
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
