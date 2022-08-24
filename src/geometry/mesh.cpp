
#include <plateau/geometry/mesh.h>

#include <memory>
#include "citygml/texture.h"

using namespace plateau::geometry;

Mesh::Mesh(const std::string &id) :
    vertices_(),
    uv1_(UV()),
    uv2_(UV()),
    uv3_(UV()),
    multiTexture_(MultiTextureURL()){
}

std::vector<TVec3d> & Mesh::getVertices() {
    return vertices_;
}

const std::vector<TVec3d> &Mesh::getVerticesConst() const {
    return vertices_;
}


const std::vector<int> & Mesh::getIndices() const {
    return indices_;
}


void Mesh::setUV2(const UV& uv2) {
    if(uv2.size() != vertices_.size()){
        std::cerr << "Size of uv2 does not match num of vertices." << std::endl;
        return;
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

const MultiTextureURL &Mesh::getMultiTexture() const {
    return multiTexture_;
}

void Mesh::Merge(const Polygon &otherPoly, const TVec2f& UV2Element, const TVec2f& UV3Element) {
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
        indices_.push_back(otherIndex);
    }
    // 追加分のインデックスを新しい値にします。前の頂点の数だけインデックスの数値を大きくすれば良いです。
    for(unsigned i=prevNumIndices; i<indices_.size(); i++){
        indices_.at(i) += prevNumVertices;
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
    // TODO ここのコードは整理したい
    // TODO テクスチャありのポリゴン → なしのポリゴン が交互にマージされることで、テクスチャなしのサブメッシュが大量に生成されるので描画負荷に改善の余地ありです。
    //      テクスチャなしのサブメッシュは1つにまとめたいところです。テクスチャなしのポリゴンを連続してマージすることで1つにまとまるはずです。
    auto otherTexture = otherPoly.getTextureFor("rgbTexture");
    std::string otherTextureURL;
    if(otherTexture == nullptr){
        otherTextureURL = std::string("");
    }else{
        otherTextureURL = otherTexture->getUrl();
    }
    bool isDifferentTex = multiTexture_.empty();
    if(!isDifferentTex){
        auto& lastTextureURL = (*multiTexture_.rbegin()).second;
        isDifferentTex |= otherTextureURL != lastTextureURL;
    }
    if( isDifferentTex ){
        multiTexture_.emplace(prevNumIndices, otherTextureURL);
    }
}