#include<plateau/polygon_mesh/sub_mesh.h>
#include <utility>
#include <stdexcept>
#include <sstream>

namespace plateau::polygonMesh {

    SubMesh::SubMesh(size_t start_index, size_t end_index, const std::string& texture_path, std::shared_ptr<const citygml::Material> material) :
            start_index_(start_index),
            end_index_(end_index),
            texture_path_(texture_path) ,
            material_(material) {}


    void
    SubMesh::addSubMesh(size_t start_index, size_t end_index, const std::string& texture_path, std::shared_ptr<const citygml::Material> material, std::vector<SubMesh>& vector) {
        if (end_index <= start_index) throw std::logic_error("addSubMesh : Index is invalid.");
        vector.emplace_back(start_index, end_index, texture_path, material);
    }

    size_t SubMesh::getStartIndex() const {
        return start_index_;
    }

    size_t SubMesh::getEndIndex() const {
        return end_index_;
    }

    const std::string& SubMesh::getTexturePath() const {
        return texture_path_;
    }

    std::shared_ptr<const citygml::Material> SubMesh::getMaterial() const {
        return material_;
    }

    void SubMesh::setTexturePath(std::string file_path) {
        texture_path_ = file_path;
    }    

    void SubMesh::setEndIndex(int end_index) {
        end_index_ = end_index;
    }

    void SubMesh::debugString(std::stringstream& ss, int indent) const {
        for (int i = 0; i < indent; i++) ss << "    ";
        ss << "SubMesh: [" << start_index_ << ", " << end_index_ << "] texturePath = '" << texture_path_ << "'"
           << std::endl;
    }
}
