#include<plateau/geometry/sub_mesh.h>

#include <utility>

using namespace plateau::geometry;

SubMesh::SubMesh(int startIndex, int endIndex, std::string texturePath) :
    startIndex(startIndex),
    endIndex(endIndex),
    texturePath(std::move(texturePath)){}


void SubMesh::addSubMesh(size_t startIndex, size_t endIndex, std::string texturePath, std::vector<SubMesh> &vector) {
    if(endIndex <= startIndex) throw std::exception("addSubMesh : index is invalid.");
    vector.emplace_back(startIndex, endIndex, std::move(texturePath));
}


