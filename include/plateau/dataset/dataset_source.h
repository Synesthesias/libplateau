#include <libplateau_api.h>
#include <filesystem>
#include "i_dataset_accessor.h"

namespace plateau::dataset {
    /**
     * PLATEAUデータ一式を表現したクラスです。
     * 保持している IDatasetAccessor によってGMLファイルの検索ができます。
     */
    class LIBPLATEAU_EXPORT DatasetSource {
    public:
        /**
         * ローカルPC内のデータを指す DatasetSource を作ります。
         */
        explicit DatasetSource(const std::filesystem::path& local_source_path);

        /**
         * サーバー上のデータを指す DatasetSource を作ります。
         * 引数に渡すべき dataset_id は、Client::getMetadata を使って
         * APIサーバーに問い合わせることで得られる Dataset の ID です。
         */
         explicit DatasetSource(const std::string& dataset_id);

         // TODO
         // 上の2つのコンストラクタのうち、pathをとるものはローカル向け、stringをとるものはサーバー向け。
         // でも2つは間違えやすそう。明示的に Server, Local という名前が付くstatic関数で
         // 分けたほうが良いのでは。

        std::shared_ptr<IDatasetAccessor> getAccessor() const;

    private:
        std::shared_ptr<IDatasetAccessor> accessor_;
    };
}
