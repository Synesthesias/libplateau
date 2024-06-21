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

    int readCharToLod(const char* found_ptr, const size_t lod_pattern_size, size_t offset, const char* const chunk_last){
        // ":lod" の次の文字 + offset を指すポインタです。そこはLODの番号を指すことを期待します。
        auto lod_num_ptr = getCharPtr(found_ptr, lod_pattern_size, offset, chunk_last);

        // ":lod" の直後の数字を求めます。 数字は1桁であることが前提です。
        int lod_num = *lod_num_ptr - u8'0';
        if (0 <= lod_num && lod_num <= 9) {
            return lod_num;
        }
        return -1;
    }

}

int LodSearcher::searchMaxLodInFile(const fs::path& file_path, int specification_max_lod) {
    auto ifs = std::ifstream(file_path);
    if (!ifs) {
        throw std::runtime_error("Failed to read file.");
    }
    return searchMaxLodInIstream(ifs, specification_max_lod);
}

int LodSearcher::searchMaxLodInIstream(std::istream& ifs, int specification_max_lod) {
    // 注意:
    // この関数は実行速度にこだわる必要があるため、 std::string よりも原始的な C言語の機能を積極的に利用しています。

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
    int found_max_lod = -1;
    std::streamsize total_read_size_from_first_lod_found = 0;
    std::streamsize total_read_size_from_second_lod_found = 0;
    std::streamsize total_read_size_from_third_lod_found = 0;

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
            // ":lod"の直後が数字であれば、そのLODを利用
            found_max_lod = std::max(readCharToLod(found_ptr, lod_pattern_size, 0, chunk_last), found_max_lod);
            // ":lod" の直後が ">" であれば、 ":lod>"の直後の数字のLODを利用 (dem向け)
            if (*getCharPtr(found_ptr, lod_pattern_size, 0, chunk_last) == u8'>'){
                found_max_lod = std::max(readCharToLod(found_ptr, lod_pattern_size, 1, chunk_last), found_max_lod);
            }

            // 最大LODが見つかった場合は探索を終了します。
            if(found_max_lod >= specification_max_lod) return found_max_lod;

            // チャンク内の次の ":lod" を探します。
            auto next_pos = std::min(found_ptr + lod_pattern_size, chunk_last);
            found_ptr = strstr(next_pos, lod_pattern);
        }

        // 最初のLODが見つかってからここまで読めば十分という値に達したとき
        total_read_size_from_first_lod_found += (found_max_lod >= 0) ? read_size : 0;
        total_read_size_from_second_lod_found += (found_max_lod >= 2) ? read_size : 0;
        total_read_size_from_third_lod_found += (found_max_lod >= 3) ? read_size : 0;
        if (
                total_read_size_from_first_lod_found > max_gml_read_size_from_first_lod_found &&
                (found_max_lod < 2 || /* LOD2→3を探すとき*/ total_read_size_from_second_lod_found > max_gml_read_size_from_second_lod_found) &&
                (found_max_lod < 3 || /* LOD3→4を探すとき*/ total_read_size_from_third_lod_found > max_gml_read_size_from_third_lod_found)
                ) {
            break;
        }
    } while (!ifs.eof());


    return found_max_lod;
}
