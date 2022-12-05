#include <plateau/dataset/dataset_source.h>
#include "local_dataset_accessor.h"
#include "server_dataset_accessor.h"

namespace plateau::dataset{
    namespace fs = std::filesystem;

    DatasetSource DatasetSource::createLocal(const std::string& local_source_path) {
        auto source = DatasetSource();
        source.accessor_ = LocalDatasetAccessor::find(local_source_path);
        return source;
    }

    DatasetSource DatasetSource::createServer(const std::string& dataset_id, const network::Client& client) {
        auto source = DatasetSource();
        const auto server_accessor = std::make_shared<ServerDatasetAccessor>(dataset_id, client);
        server_accessor->loadFromServer();
        source.accessor_ = server_accessor;
        return source;
    }

    std::shared_ptr<IDatasetAccessor> DatasetSource::getAccessor() const {
        return accessor_;
    }
}
