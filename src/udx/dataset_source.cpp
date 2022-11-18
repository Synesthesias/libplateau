#include <plateau/udx/dataset_source.h>
#include <plateau/udx/local_dataset_accessor.h>

namespace plateau::udx{
    namespace fs = std::filesystem;
    DatasetSource::DatasetSource(const fs::path& local_source_path) {
        accessor_ = std::make_shared<LocalDatasetAccessor>();
        LocalDatasetAccessor::find(local_source_path.string(), *((LocalDatasetAccessor*)(accessor_.get())));
    }

    std::shared_ptr<IDatasetAccessor> DatasetSource::getAccessor() const {
        return accessor_;
    }
}
