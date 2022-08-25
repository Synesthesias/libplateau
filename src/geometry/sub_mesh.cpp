#include<plateau/geometry/sub_mesh.h>
#include <utility>

using namespace plateau::geometry;

SubMesh::SubMesh(size_t startIndex, size_t endIndex, std::string texturePath) :
        startIndex_(startIndex),
        endIndex_(endIndex),
        texturePath_(std::move(texturePath)){}

void SubMesh::addSubMesh(size_t startIndex, size_t endIndex, std::string texturePath, std::vector<SubMesh> &vector) {
    if(endIndex <= startIndex) throw std::exception("addSubMesh : Index is invalid.");
    vector.emplace_back(startIndex, endIndex, std::move(texturePath));
}

size_t SubMesh::getStartIndex() const {
    return startIndex_;
}

size_t SubMesh::getEndIndex() const {
    return endIndex_;
}

const std::string& SubMesh::getTexturePath() const{
    return texturePath_;
}

void SubMesh::setEndIndex(int endIndex) {
    endIndex_ = endIndex;
}
