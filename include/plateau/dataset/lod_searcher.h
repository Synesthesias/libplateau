#pragma once

#include <filesystem>
#include <libplateau_api.h>

namespace plateau::dataset {

    struct LIBPLATEAU_EXPORT LodFlag;

    /**
     * \brief GMLファイルに含まれるLOD番号を検索します。
     * ファイルの中身を文字列検索し、":lod(番号)" にヒットした番号のうち高いものを返します。
     *
     * 用途:
     * PLATEAUデータのインポートにおける範囲選択画面で、GMLファイルについて利用可能なLODを検索します。
     * 範囲選択画面では多くのGMLファイルを検索対象とするので、高速である必要があります。
     *
     * 検証方法:
     * このクラスに変更を加えたとき、多くのGMLファイルに関して結果が変わらないかどうかを検証する方法を残しました。
     * 詳しくはtest_lod_searcher.cpp の DisplayLodsRecursive という名前のテストのコメントを参照してください。
     */
    class LIBPLATEAU_EXPORT LodSearcher {

    public:
        /// ファイル中に含まれる最大LODを返します。
        /// 引数 specification_max_lod は仕様上とりうるLODの最大値であり、そのLODが見つかった場合は探索を終了してその値を返します。
        static int searchMaxLodInFile(const std::filesystem::path& file_path, int specification_max_lod);
        static int searchMaxLodInIstream(std::istream& ifs, int specification_max_lod);

    private:

        /**
         * GMLファイルの冒頭の何メガバイトを検索対象とするかの値です。その値以降は検索しません。
         * サイズを制限しないと、範囲選択画面で重いGMLファイルのアイコンを1つ表示するのに長々と待たされることになります。
         * 制限が50MBであれば、2023年の東京、沼津、新潟の全データにおいて、全文を読むのと結果が変わらないことを検証済みです。
         * しかし、今後のあらゆるデータで大丈夫であるという保証はなく、今後に調整が必要になるかもしれません。
         */
        static constexpr long max_gml_read_size_from_file_start = 50 /*メガバイト*/* 1000000;
        // サイズ制限を無くしたい場合は下のコメントアウトを外してください
//        static constexpr long max_gml_read_size = LONG_MAX;

        static constexpr long max_gml_read_size_from_first_lod_found = 5 /*メガバイト*/*1000000;
    };
}
