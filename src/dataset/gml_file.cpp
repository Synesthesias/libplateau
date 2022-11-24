#include <string>
#include <filesystem>
#include <regex>
#include <fstream>
#include <set>
#include <plateau/dataset/gml_file.h>
#include <plateau/dataset/mesh_code.h>

namespace plateau::dataset {

    namespace fs = std::filesystem;

    GmlFile::GmlFile(const std::string& path)
        : path_(path),
        is_valid_(false){
        applyPath();
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
        for (const auto& character : filename) {
            appearance_path += character;
            if (character == '_') {
                cnt++;
                if (cnt == 3)
                    break;
            }
        }
        appearance_path += "appearance";
        return appearance_path;
    }

    bool GmlFile::isValid() const {
        return is_valid_;
    }

    void GmlFile::applyPath() {
        const auto filename = fs::u8path(path_).filename().u8string();
        std::vector<std::string> filename_parts;
        std::string current;
        current.reserve(filename.size());
        for (const auto character : filename) {
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
                        str_begin + std::max((long long)0, (long long)hint_matched_pos - search_range_before_hint);
                auto search_end = std::min(str.end(), str_begin + (long long)hint_matched_pos + (long long)hint.size() + search_range_after_hint);
                // 正規表現でヒットしたら、その結果を引数 matched に格納して返します。
                bool found = std::regex_search(search_start, search_end, matched, regex);
                if (found) return true;
                // ヒントにはヒットしたけど正規表現にヒットしなかったケースです。検索位置を進めて再度ヒントを検索します。
                search_pos = std::min(str.cend(), str_begin + (long long)hint_matched_pos + (long long)hint.size());
            }

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

        std::string loadFile(const fs::path& file_path) {
            std::ifstream ifs(file_path.u8string());
            if (!ifs) {
                throw std::runtime_error(
                        "loadFile : Could not open file " + file_path.u8string());
            }
            std::ostringstream buffer;
            buffer << ifs.rdbuf();
            return buffer.str();
        }

        auto regex_options = std::regex::optimize | std::regex::nosubs;

        std::set<std::string> searchAllImagePathsInGML(const std::string& file_content) {
            // 開始タグは <app:imageURI> です。ただし、<括弧> の前後に半角スペースがあっても良いものとします。
            static const auto begin_tag = std::regex(R"(< *app:imageURI *>)", regex_options);
            // 終了タグは </app:imageURI> です。ただし、<括弧> と /(スラッシュ) の前後に半角スペースがあっても良いものとします。
            static const auto end_tag = std::regex(R"(< */ *app:imageURI *>)", regex_options);
            static auto tag_hint = std::string("app:imageURI");
            auto found_url_strings = searchAllStringsBetween(begin_tag, end_tag, file_content, tag_hint, tag_hint, 5, 10);
            return found_url_strings;
        }

        std::set<std::string> searchAllCodelistPathsInGML(const std::string& file_content) {
            // 開始タグは codeSpace=" です。ただし =(イコール), "(ダブルクォーテーション)の前後に半角スペースがあっても良いものとします。
            static const auto begin_tag = std::regex(R"(codeSpace *= *")", regex_options);
            // 終了タグは、開始タグの次の "(ダブルクォーテーション)です。
            static const auto end_tag = std::regex(R"(")", regex_options);
            static const auto begin_tag_hint = "codeSpace";
            auto found_strings = searchAllStringsBetween(begin_tag, end_tag, file_content, begin_tag_hint, "\"", 5, 10);
            return found_strings;
        }

        /**
         * 引数の set の中身を相対パスと解釈し、 setの各要素をコピーします。
         * 相対パスの基準は コピー元は 引数 src_base_path、 コピー先は dest_base_path になります。
         * コピー先に同名のフォルダが存在する場合はコピーしません。
         * コピー元が実在しない場合はコピーしません。
         */
        void copyFiles(const std::set<std::string>& path_set, const fs::path& src_base_path, const fs::path& dest_base_path) {
            for (const auto& path : path_set) {
                auto src = src_base_path;
                auto dest = dest_base_path;
                src.append(path).make_preferred();
                dest.append(path).make_preferred();
                if (!fs::exists(src)) {
                    std::cout << "file not exist : " << src.string() << std::endl;
                    continue;
                }
                fs::create_directories(dest.parent_path());
                fs::copy(src, dest, fs::copy_options::skip_existing);
            }
        }
    } // fetch で使う無名関数

    std::shared_ptr<GmlFile> GmlFile::fetch(const std::string& destination_root_path) const {
        auto result = std::make_shared<GmlFile>("");
        fetch(destination_root_path, *result);
        return result;
    }

    void GmlFile::fetch(const std::string& destination_root_path, GmlFile& copied_gml_file) const {

        const auto udx_path_len = getPath().rfind("udx") + 3;
        if (udx_path_len == std::string::npos) {
            throw std::runtime_error("Invalid gml path. Could not find udx folder");
        }

        const auto udx_path = getPath().substr(0, udx_path_len);
        const auto gml_relative_path = fs::relative(fs::u8path(getPath()).make_preferred(), fs::u8path(udx_path)).make_preferred().string();

        const auto root_folder_name = fs::u8path(udx_path).parent_path().filename().string();
        auto destination_root = fs::u8path(destination_root_path);
        const auto destination_udx_path = destination_root.append(root_folder_name).append("udx").string();
        fs::path gml_destination_path(destination_udx_path);
        gml_destination_path.append(gml_relative_path);
        fs::create_directories(gml_destination_path.parent_path());
        const auto& gml_file_path = getPath();
        try {
            fs::copy(gml_file_path, gml_destination_path, fs::copy_options::skip_existing);
        }
        catch (...) {
        }

        // GMLファイルを読み込み、関連するテクスチャパスとコードリストパスを取得します。
        const auto gml_content = loadFile(getPath());
        auto image_paths = searchAllImagePathsInGML(gml_content);
        auto codelist_paths = searchAllCodelistPathsInGML(gml_content);

        for (const auto& path : image_paths) {
            std::cout << path << std::endl;
        }
        for (const auto& path : codelist_paths) {
            std::cout << path << std::endl;
        }

        // テクスチャとコードリストファイルをコピーします。
        auto gml_dir_path = fs::path(gml_file_path).parent_path();
        auto relative_gml_dir_path = fs::path(gml_relative_path).parent_path().u8string();
        auto app_destination_path = fs::path(destination_udx_path).append(relative_gml_dir_path);
        copyFiles(image_paths, gml_dir_path, app_destination_path);
        copyFiles(codelist_paths, gml_dir_path, app_destination_path);

        copied_gml_file.setPath(gml_destination_path.u8string());
    }
}
