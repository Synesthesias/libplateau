#include <string>
#include <filesystem>
#include <regex>
#include <fstream>
#include <set>
#include <utility>

#include <plateau/dataset/gml_file.h>
#include <plateau/dataset/mesh_code.h>
#include <plateau/network/client.h>

#include "lod_searcher.h"

using namespace plateau::network;
namespace fs = std::filesystem;

namespace {
    bool checkLocal(const fs::path& path) {
        auto path_str = path.u8string();
        if (path_str.size() < 4)
            return true;
        return path_str.substr(0, 4) != "http";
    }
}

namespace plateau::dataset {

    GmlFile::GmlFile(const std::string& path) // NOLINT
            : path_(path), is_valid_(false), is_local_(true), max_lod_(-1) {
        // 上の行の is_valid_ と is_local_ の設定は一時的なもので、すぐ applyPath() によって書き換えられます。
        applyPath();
    }

    /// サーバーモードで使うコンストラクタです。GMLファイルのダウンロードに使う Client を指定します。
    GmlFile::GmlFile(const std::string& path, const network::Client& client, int max_lod)
    : GmlFile(path){
        client_ = client;
        max_lod_ = max_lod;
    }

    const std::string& GmlFile::getPath() const {
        return path_;
    }

    void GmlFile::setPath(const std::string& path) {
        path_ = path;
        applyPath();
    }

    MeshCode GmlFile::getMeshCode() const {
        return MeshCode(code_);
    }

    const std::string& GmlFile::getFeatureType() const {
        return feature_type_;
    }

    std::string GmlFile::getAppearanceDirectoryPath() const {
        const auto filename = fs::u8path(path_).filename().u8string();
        auto appearance_path = fs::u8path(path_).parent_path().append("").u8string();
        std::string current;
        int cnt = 0;
        for (const auto& character: filename) {
            appearance_path += character;
            if (character == '_') {
                cnt++;
                if (cnt == 3)
                    break;
            }
        }
        appearance_path += u8"appearance";
        return appearance_path;
    }

    int GmlFile::getMaxLod() {
        if (isMaxLodCalculated())
            return max_lod_;

        auto lods = LodSearcher::searchLodsInFile(fs::u8path(path_));
        max_lod_ = lods.getMax();
        if(max_lod_ < 0) max_lod_ = 0; // MaxLodが取得できなかった場合のフェイルセーフです。
        return max_lod_;
    }

    bool GmlFile::isValid() const {
        return is_valid_ && getMeshCode().isValid();
    }

    bool GmlFile::isMaxLodCalculated() const {
        return max_lod_ >= 0;
    }

    void GmlFile::applyPath() {
        auto path = fs::u8path(path_);
        is_local_ = checkLocal(path);

        const auto filename = path.filename().u8string();
        std::vector<std::string> filename_parts;
        std::string current;
        current.reserve(filename.size());
        for (const auto character: filename) {
            if (character == '_') {
                filename_parts.push_back(current);
                current = "";
                continue;
            }
            current += character;
        }
        try {
            code_ = filename_parts.empty() ? "" : filename_parts.at(0);
            feature_type_ = filename_parts.size() <= 1 ? "" : filename_parts.at(1);
            is_valid_ = true;
        }
        catch (...) {
            is_valid_ = false;
        }
    }


    // fetch で使う無名関数
    namespace {
        using ConstStrIterT = decltype(std::string("a").cbegin());

        /**
        * @brief 正規表現で検索しますが、ヒントを与えることで検索を高速化します。
        *        ヒントとは何かについては、 関数 searchAllStringsBetween のコメントを参照してください。
        * @param str 検索対象の文字列です。
        * @param search_pos 検索の開始位置です。
        * @param matched 検索結果はここに格納されます。
        * @param regex 検索する正規表現です。
        * @param hint ヒント文字列です。正規表現が検索ヒットするとき、ヒント文字列が必ず検索ヒットし、
        *             かつそのヒント文字列の周囲の指定バイト数にその正規表現ヒット文字列が含まれることが条件となります。
        * @param search_range_before_hint ヒント文字列の前の何バイト目から正規表現による検索を始めるかです。
        * @param search_range_after_hint ヒント文字列の後の何バイト目まで正規表現による検索対象にするかです。
        * @return ヒットしたらtrue, なければ false を返します。
        */
        bool regexSearchWithHint(const std::string& str, ConstStrIterT search_pos, std::smatch& matched,
                                 const std::regex& regex, const std::string& hint,
                                 unsigned search_range_before_hint, unsigned search_range_after_hint
        ) {
            const auto str_begin = str.cbegin();
            while (search_pos != str.cend()) {
                // ヒントで検索します。
                auto hint_matched_pos = str.find(hint, search_pos - str_begin);
                // ヒントで検索ヒットしなければ、正規表現でも検索ヒットしません。そのようなヒントが渡されていることが前提です。
                if (hint_matched_pos == std::string::npos) return false;
                // ヒントが検索ヒットしたので、その周囲の指定数のバイト範囲を正規表現の検索範囲にします。
                auto search_start =
                        str_begin + std::max((long long) 0, (long long) hint_matched_pos - search_range_before_hint);
                auto search_end = std::min(str.end(),
                                           str_begin + (long long) hint_matched_pos + (long long) hint.size() +
                                           search_range_after_hint);
                // 正規表現でヒットしたら、その結果を引数 matched に格納して返します。
                bool found = std::regex_search(search_start, search_end, matched, regex);
                if (found) return true;
                // ヒントにはヒットしたけど正規表現にヒットしなかったケースです。検索位置を進めて再度ヒントを検索します。
                search_pos = std::min(str.cend(), str_begin + (long long) hint_matched_pos + (long long) hint.size());
            }
            return false;
        }

        /**
        * 引数文字列 str のうち、引数 begin_tag_regex と end_tag_regex で囲まれた文字列をすべて検索し set で返します。
        * end_tag_regex は begin_tag_regex が登場する箇所より後が検索対象となります。
        * begin_tag_regex に対応する end_tag_regex がない場合、strの末尾までが対象となります。
        * 検索結果のうち同じ文字列は1つにまとめられます。
        *
        * ヒントについて:
        * 検索の高速化のために引数でヒント文字列を与える必要があります。
        * 例えば40MBのGMLファイルに対して愚直に正規表現で検索すると1分30秒程度の時間がかかります。
        * 引数でヒントを与えることで、正規表現の検索範囲が狭まり 4秒程度に短縮できました。
        * ヒント文字列とは、正規表現が検索ヒットするとき、その場所でヒント文字列も必ずヒットする、という条件を満たす文字列です。
        * 例えば、文字列 <start_tag> を検索したいが、<括弧> の前後に半角スペースが入っているケースも検索したいという場合、
        * 検索正規表現は < *start_tag *> であり、ヒント文字列は start_tag となります。
        * このとき、まず正規表現検索よりも高速な通常文字列検索で start_tag が検索されます。
        * そして見つかった位置から前後に指定バイト数の幅をとった範囲を正規表現で検索します。
        * その範囲は引数 search_range_before_hint, search_range_after_hint で指定します。この値は少ないほうが速くなります。
        */
        std::set<std::string> searchAllStringsBetween(
                const std::regex& begin_tag_regex, const std::regex& end_tag_regex,
                const std::string& str,
                const std::string& begin_tag_hint, const std::string& end_tag_hint,
                unsigned search_range_before_hint, unsigned search_range_after_hint
        ) {
            std::set<std::string> found;
            std::smatch begin_tag_matched;
            std::smatch end_tag_matched;
            auto begin_tag_search_iter = str.begin();
            while (true) {
                // 開始タグを検索します。
                if (!regexSearchWithHint(str, begin_tag_search_iter, begin_tag_matched, begin_tag_regex, begin_tag_hint,
                                         search_range_before_hint, search_range_after_hint)) {
                    break;
                }
                // 終了タグを検索します。
                const auto next_of_begin_tag = begin_tag_matched[0].second;
                if (regexSearchWithHint(str, next_of_begin_tag, end_tag_matched, end_tag_regex, end_tag_hint,
                                        search_range_before_hint, search_range_after_hint)) {
                    // 開始タグと終了タグに挟まれた文字列を結果として格納します。
                    found.insert(std::string(next_of_begin_tag, end_tag_matched[0].first));
                } else {
                    found.insert(std::string(next_of_begin_tag, str.end()));
                    break;
                }
                const auto next_of_end_tag = end_tag_matched[0].second;
                // イテレーターを進めます。
                begin_tag_search_iter = next_of_end_tag;
            }
            return found;
        }

        // TODO GMLファイルの全文をメモリにコピーするので重いです。LodSearcher::searchLOD の実装を参考にすると速くなりそうです。
        std::string loadFile(const fs::path& file_path) {
            std::ifstream ifs(file_path);
            if (!ifs) {
                throw std::runtime_error(
                        "loadFile : Could not open file " + file_path.u8string());
            }
            std::ostringstream buffer;
            buffer << ifs.rdbuf();
            return buffer.str();
        }

        auto regex_options = std::regex::optimize | std::regex::nosubs;

        /**
         * 引数の set の中身を相対パスと解釈し、 setの各要素をコピーします。
         * 相対パスの基準は コピー元は 引数 src_base_path、 コピー先は dest_base_path になります。
         * コピー先に同名のフォルダが存在する場合はコピーしません。
         * コピー元が実在しない場合はコピーしません。
         */
        void copyFiles(const std::set<std::string>& path_set, const fs::path& src_base_path,
                       const fs::path& dest_base_path) {
            for (const auto& path: path_set) {
                auto src = fs::path(src_base_path).append(path).make_preferred();
                auto dest = fs::path(dest_base_path).append(path).make_preferred();
                if (!fs::exists(src)) {
                    std::cout << "file not exist : " << src.u8string() << std::endl;
                    continue;
                }
                fs::create_directories(dest.parent_path());
                fs::copy(src, dest, fs::copy_options::skip_existing);
            }
        }

        /**
         * fetch の準備として、パスを計算し、必要なディレクトリを作成します。
         **/
        void prepareFetch(const fs::path& gml_path, const fs::path& destination_root_path,
                          fs::path& out_gml_relative_path_from_udx, fs::path& out_destination_udx_path,
                          fs::path& out_gml_destination_path) {
            auto gml_path_str = gml_path.u8string();
            const auto udx_path_len = gml_path_str.rfind(u8"udx") + 3;
            if (udx_path_len == std::string::npos) {
                throw std::runtime_error("Invalid gml path. Could not find udx folder");
            }

            const auto udx_path = gml_path_str.substr(0, udx_path_len);
            out_gml_relative_path_from_udx = fs::relative(fs::path(gml_path).make_preferred(),
                                                          fs::u8path(udx_path)).make_preferred().u8string();
            const auto root_folder_name = fs::u8path(udx_path).parent_path().filename();
            out_destination_udx_path = (fs::path(destination_root_path) / root_folder_name).append(u8"udx");
            out_gml_destination_path = fs::path(out_destination_udx_path);
            out_gml_destination_path /= out_gml_relative_path_from_udx;
            fs::create_directories(out_gml_destination_path.parent_path());
        }

        void fetchLocal(const fs::path& gml_file_path, const fs::path& gml_relative_path_from_udx, const fs::path& destination_udx_path,
                        const fs::path& gml_destination_path, GmlFile& copied_gml_file) {
            auto destination_dir = gml_destination_path.parent_path();
            fs::copy(gml_file_path, gml_destination_path, fs::copy_options::skip_existing);
            copied_gml_file.setPath(gml_destination_path.u8string());

            // GMLファイルを読み込み、関連するテクスチャパスとコードリストパスを取得します。
            const auto codelist_paths = copied_gml_file.searchAllCodelistPathsInGML();
            const auto image_paths = copied_gml_file.searchAllImagePathsInGML();

            // テクスチャとコードリストファイルをコピーします。
            const auto gml_dir_path = gml_file_path.parent_path();
            const auto relative_gml_dir_path = gml_relative_path_from_udx.parent_path().u8string();
            const auto app_destination_path = fs::path(destination_udx_path) / relative_gml_dir_path;
            auto path_to_download = image_paths;
            path_to_download.insert(codelist_paths.cbegin(), codelist_paths.cend());
            copyFiles(path_to_download, gml_dir_path, app_destination_path);
        }

        void fetchServer(const fs::path& gml_file_path, const fs::path& gml_relative_path_from_udx, const fs::path& destination_udx_path,
                         const fs::path& gml_destination_path, GmlFile& copied_gml_file, const network::Client& client) {
            auto destination_dir = gml_destination_path.parent_path();
            // gmlファイルをダウンロードします。
            client.download(destination_dir.u8string(), gml_file_path.u8string());
            auto downloaded_path = destination_dir;
            downloaded_path /= gml_file_path.filename();
            copied_gml_file.setPath(downloaded_path.u8string());

            // GMLファイルを読み込み、関連するテクスチャパスとコードリストパスを取得します。
            const auto codelist_paths = copied_gml_file.searchAllCodelistPathsInGML();
            const auto image_paths = copied_gml_file.searchAllImagePathsInGML();

            // テクスチャとコードリストファイルをダウンロードします。
            const auto gml_dir_path = gml_file_path.parent_path();
            const auto relative_gml_dir_path = gml_relative_path_from_udx.parent_path().u8string();
            const auto app_destination_path = fs::path(destination_udx_path) / relative_gml_dir_path;
            auto path_to_download = image_paths;
            path_to_download.insert(codelist_paths.cbegin(), codelist_paths.cend());


            for (const auto& relative_path: path_to_download) {
                auto full_path = fs::absolute(fs::path(destination_dir) / fs::u8path(relative_path));
                auto download_path = (gml_dir_path / fs::path(relative_path)).lexically_normal().u8string();
                std::replace(download_path.begin(), download_path.end(), '\\', '/');

                // 上の lexically_normal の副作用で "http(s)://" が "http(s):/" になるので、
                // 欠けたスラッシュを1つ追加します。
                auto search = std::string(":/");
                auto pos = download_path.find(search);
                auto len = search.length();
                if(pos == 4 || pos == 5){
                    download_path.replace(pos, len, "://");
                }

                client.download(full_path.parent_path().u8string(), download_path);
            }
        }
    } // fetch で使う無名関数

    std::shared_ptr<GmlFile> GmlFile::fetch(const std::string& destination_root_path) const {
        auto result = std::make_shared<GmlFile>(std::string(""));
        fetch(destination_root_path, *result);
        return result;
    }

    void GmlFile::fetch(const std::string& destination_root_path, GmlFile& copied_gml_file) const {
        if(!isValid()) throw std::runtime_error("gml file is invalid.");
        auto gml_relative_path_from_udx = fs::path();
        auto destination_udx_path = fs::path();
        auto gml_destination_path = fs::path();
        prepareFetch(fs::u8path(getPath()), fs::u8path(destination_root_path), gml_relative_path_from_udx, destination_udx_path,
                     gml_destination_path);
        if (is_local_) {
            // ローカルモード
            fetchLocal(fs::u8path(path_), gml_relative_path_from_udx, destination_udx_path, gml_destination_path,
                       copied_gml_file);
        } else {
            // サーバーモード
            if (!client_.has_value()) {
                throw std::runtime_error("Client is nullopt.");
            }
            fetchServer(fs::u8path(path_), gml_relative_path_from_udx, destination_udx_path, gml_destination_path,
                        copied_gml_file, client_.value());
        }
    }

    std::set<std::string> GmlFile::searchAllCodelistPathsInGML() const {
        const auto gml_content = loadFile(fs::u8path(getPath()));
        // 開始タグは codeSpace=" です。ただし =(イコール), "(ダブルクォーテーション)の前後に半角スペースがあっても良いものとします。
        static const auto begin_tag = std::regex(R"(codeSpace *= *")", regex_options);
        // 終了タグは、開始タグの次の "(ダブルクォーテーション)です。
        static const auto end_tag = std::regex(R"(")", regex_options);
        static const auto begin_tag_hint = "codeSpace";
        auto codelist_paths = searchAllStringsBetween(begin_tag, end_tag, gml_content, begin_tag_hint, "\"", 5, 10);
        return codelist_paths;
    }

    std::set<std::string> GmlFile::searchAllImagePathsInGML() const {
        const auto gml_content = loadFile(fs::u8path(getPath()) );
        // 開始タグは <app:imageURI> です。ただし、<括弧> の前後に半角スペースがあっても良いものとします。
        static const auto begin_tag = std::regex(R"(< *app:imageURI *>)", regex_options);
        // 終了タグは </app:imageURI> です。ただし、<括弧> と /(スラッシュ) の前後に半角スペースがあっても良いものとします。
        static const auto end_tag = std::regex(R"(< */ *app:imageURI *>)", regex_options);
        static auto tag_hint = std::string("app:imageURI");
        auto image_paths = searchAllStringsBetween(begin_tag, end_tag, gml_content, tag_hint, tag_hint, 5, 10);
        return image_paths;
    }
}
