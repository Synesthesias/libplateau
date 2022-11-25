#include <plateau/dataset/dataset_source.h>
#include <plateau/dataset/local_dataset_accessor.h>
#include <plateau/dataset/server_dataset_accessor.h>

namespace plateau::dataset{
    namespace fs = std::filesystem;

    DatasetSource::DatasetSource(const fs::path& local_source_path) {
        accessor_ = std::make_shared<LocalDatasetAccessor>();
        LocalDatasetAccessor::find(local_source_path.string(), *((LocalDatasetAccessor*)(accessor_.get())));
    }

    DatasetSource::DatasetSource(const std::string& dataset_id) {
        accessor_ = std::make_shared<ServerDatasetAccessor>(dataset_id);
    }

    std::shared_ptr<IDatasetAccessor> DatasetSource::getAccessor() const {
        return accessor_;
    }
}
