// TODO あとで消す
//#include <plateau/udx/dataset_accessor_p_invoke.h>
//#include <exception>
//#include <utility>
//
//namespace plateau::udx {
//
//    DatasetAccessorPInvoke::DatasetAccessorPInvoke(std::shared_ptr<IDatasetAccessor> accessor) :
//            accessor_(std::move(accessor)) {
//    }
//
//    void DatasetAccessorPInvoke::getGmlFiles(geometry::Extent extent, PredefinedCityModelPackage package) {
//        result_of_get_gml_files_[package] = std::vector<GmlFile*>();
//        auto gml_files = accessor_->getGmlFiles(extent, package);
//        // GmlFile の delete は DLL利用者の責任とするため、ヒープに移します。
//        for(auto& gml : gml_files){
//            auto heap_gml = new GmlFile(std::move(gml));
//            result_of_get_gml_files_.at(package).push_back(heap_gml);
//        }
//    }
//
//    const GmlFile* DatasetAccessorPInvoke::resultOfGetGmlFiles(PredefinedCityModelPackage package, int index) const {
//        const auto& package_files = result_of_get_gml_files_.at(package);
//        if(index >= package_files.size() || index < 0){
//            throw std::out_of_range("index is out of range.");
//        }
//        return package_files.at(index);
//    }
//
//    int DatasetAccessorPInvoke::resultOfGetGmlFilesCount(PredefinedCityModelPackage package) const {
//        return (int)result_of_get_gml_files_.at(package).size();
//    }
//
//    void DatasetAccessorPInvoke::getMeshCodes() {
//        result_of_get_mesh_codes_ = accessor_->getMeshCodes();
//    }
//
//    MeshCode DatasetAccessorPInvoke::resultOfGetMeshCodes(int index) const {
//        if(index >= result_of_get_mesh_codes_.size()) throw std::out_of_range("index is out of range.");
//        return result_of_get_mesh_codes_.at(index);
//    }
//
//    int DatasetAccessorPInvoke::resultOfGetMeshCodesCount() const {
//        return (int)result_of_get_mesh_codes_.size();
//    }
//
//    int DatasetAccessorPInvoke::getMaxLod(MeshCode mesh_code, PredefinedCityModelPackage package) {
//        return accessor_->getMaxLod(mesh_code, package);
//    }
//}
