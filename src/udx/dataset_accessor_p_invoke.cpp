#include <plateau/udx/dataset_accessor_p_invoke.h>
#include <exception>
#include <utility>

namespace plateau::udx {

    DatasetAccessorPInvoke::DatasetAccessorPInvoke(std::shared_ptr<IDatasetAccessor> accessor) :
            accessor_(std::move(accessor)) {
    }

    void DatasetAccessorPInvoke::getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package) {
        result_of_get_gml_files_ = accessor_->getGmlFiles(extent, package);
    }

    const GmlFileInfo* DatasetAccessorPInvoke::resultOfGetGmlFiles(int index) const {
        if(index >= result_of_get_gml_files_.size() || index < 0){
            throw std::out_of_range("index is out of range.");
        }
        return &(result_of_get_gml_files_.at(index));
    }

    int DatasetAccessorPInvoke::resultOfGetGmlFilesCount() const {
        return (int)result_of_get_gml_files_.size();
    }

    void DatasetAccessorPInvoke::getMeshCodes() {
        result_of_get_mesh_codes_ = accessor_->getMeshCodes();
    }

    MeshCode DatasetAccessorPInvoke::resultOfGetMeshCodes(int index) const {
        if(index >= result_of_get_mesh_codes_.size()) throw std::out_of_range("index is out of range.");
        return result_of_get_mesh_codes_.at(index);
    }

    int DatasetAccessorPInvoke::resultOfGetMeshCodesCount() const {
        return (int)result_of_get_mesh_codes_.size();
    }
}
