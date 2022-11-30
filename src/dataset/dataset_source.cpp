#include <plateau/dataset/dataset_source.h>
#include <plateau/dataset/local_dataset_accessor.h>
#include <plateau/dataset/server_dataset_accessor.h>

namespace plateau::dataset{
    namespace fs = std::filesystem;

    DatasetSource DatasetSource::createLocal(const fs::path& local_source_path) {
        auto source = DatasetSource();
        source.accessor_ = std::make_shared<LocalDatasetAccessor>();
        LocalDatasetAccessor::find(local_source_path.string(), *((LocalDatasetAccessor*)(source.accessor_.get())));
        return source;
    }

    DatasetSource DatasetSource::createServer(const std::string& dataset_id) {
        auto source = DatasetSource();
        source.accessor_ = std::make_shared<ServerDatasetAccessor>(dataset_id);
        return source;
    }

    std::shared_ptr<IDatasetAccessor> DatasetSource::getAccessor() const {
        return accessor_;
    }
}
