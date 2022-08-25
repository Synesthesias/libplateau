#include <plateau/geometry/mesh.h>
#include <memory>
#include "citygml/texture.h"

using namespace plateau::geometry;

Mesh::Mesh(const std::string &id) :
    vertices_(),
    uv1_(UV()),
    uv2_(UV()),
    uv3_(UV()),
    subMeshes_(){
}

std::vector<TVec3d>& Mesh::getVertices() {
    return vertices_;
}

const std::vector<TVec3d>& Mesh::getVerticesConst() const {
    return vertices_;
}

const std::vector<int>& Mesh::getIndices() const {
    return indices_;
}

void Mesh::setUV2(const UV& uv2) {
    if(uv2.size() != vertices_.size()){
        throw std::exception("Size of uv2 does not match num of vertices.");
    }
    uv2_ = uv2;
}

const UV& Mesh::getUV1() const{
    return uv1_;
}

const UV& Mesh::getUV2() const{
    return uv2_;
}

const UV& Mesh::getUV3() const{
    return uv3_;
}

const std::vector<SubMesh>& Mesh::getSubMeshes() const {
    return subMeshes_;
}

void Mesh::Merge(const Polygon& otherPoly, const TVec2f& UV2Element, const TVec2f& UV3Element) {
    if(otherPoly.getVertices().empty()) return;
    unsigned prevNumVertices = vertices_.size();
    unsigned prevNumIndices = indices_.size();
    auto& otherVertices = otherPoly.getVertices();
    auto& otherIndices = otherPoly.getIndices();
    // 頂点リストの末尾に追加します。
    for(const auto & otherVertex : otherVertices){
        vertices_.push_back(otherVertex);
    }
    // インデックスリストの末尾に追加します。
    for(unsigned int otherIndex : otherIndices){
        indices_.push_back((int)otherIndex);
    }
    // 追加分のインデックスを新しい値にします。前の頂点の数だけインデックスの数値を大きくすれば良いです。
    for(unsigned i=prevNumIndices; i<indices_.size(); i++){
        indices_.at(i) += (int)prevNumVertices;
    }

    // UV1を追加します。
    auto& otherUV1 = otherPoly.getTexCoordsForTheme("rgbTexture", true);
    for(const auto & vec : otherUV1){
        uv1_.push_back(vec);
    }
    // otherUV1 の数が頂点数に足りなければ 0 で埋めます
    for(size_t i = otherUV1.size(); i < otherVertices.size(); i++){
        uv1_.emplace_back(0,0);
    }

    // UV2,UV3を追加します。UV値は追加分の頂点ですべて同じ値を取ります。
    for(int i=0; i<otherVertices.size(); i++){
        uv2_.push_back(UV2Element);
        uv3_.push_back(UV3Element);
    }

    // テクスチャが異なる場合は追加します。
    // TODO テクスチャありのポリゴン → なしのポリゴン が交互にマージされることで、テクスチャなしのサブメッシュが大量に生成されるので描画負荷に改善の余地ありです。
    //      テクスチャなしのサブメッシュは1つにまとめたいところです。テクスチャなしのポリゴンを連続してマージすることで1つにまとまるはずです。
    auto otherTexture = otherPoly.getTextureFor("rgbTexture");
    std::string otherTexturePath;
    if(otherTexture == nullptr){
        otherTexturePath = std::string("");
    }else{
        otherTexturePath = otherTexture->getUrl();
    }

    // 前と同じテクスチャかどうか判定します。
    bool isDifferentTex;
    if(subMeshes_.empty()){
        isDifferentTex = true;
    }else{
        auto& lastTexturePath = subMeshes_.rbegin()->getTexturePath();
        isDifferentTex = otherTexturePath != lastTexturePath;
    }

    if( isDifferentTex ){
        // テクスチャが違うなら、サブメッシュを追加します。
        SubMesh::addSubMesh(prevNumIndices, indices_.size()-1, otherTexturePath, subMeshes_);
    }else{
        // テクスチャが同じなら、最後のサブメッシュの範囲を延長して新しい部分の終わりに合わせます。
        subMeshes_.at(subMeshes_.size()-1).setEndIndex((int)indices_.size()-1);
    }
}

