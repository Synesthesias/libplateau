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
        DatasetSource(const std::filesystem::path& local_source_path);
        std::shared_ptr<IDatasetAccessor> getAccessor() const;

    private:
        std::shared_ptr<IDatasetAccessor> accessor_;
    };
}
