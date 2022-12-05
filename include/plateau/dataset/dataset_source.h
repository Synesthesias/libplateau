#include <libplateau_api.h>
#include <filesystem>
#include "i_dataset_accessor.h"

namespace plateau::dataset {
    /**
     * PLATEAUデータ一式を表現したクラスです。
     * 保持している IDatasetAccessor によってGMLファイルの検索ができます。
     * データの場所が ローカルPCか APIサーバーかは DatasetSource の作成時にはっきりさせたいので、
     * デフォルトコンストラクタの代わりに createLocal または createServer を利用してください。
     * 表記例: new DatasetSource(DatasetSource::createServer("23ku"));
     */
    class LIBPLATEAU_EXPORT DatasetSource {
    public:

        /**
         * ローカルPC内のデータを指す DatasetSource を作ります。
         */
        static DatasetSource createLocal(const std::filesystem::path& local_source_path);

        /**
         * サーバー上のデータを指す DatasetSource を作ります。
         * 引数に渡すべき dataset_id は、Client::getMetadata を使って
         * APIサーバーに問い合わせることで得られる Dataset の ID です。
         */
        static DatasetSource createServer(const std::string& dataset_id);

        std::shared_ptr<IDatasetAccessor> getAccessor() const;

    private:
        DatasetSource() = default;
        std::shared_ptr<IDatasetAccessor> accessor_;
    };
}
