
#include <plateau/geometry/mesh.h>

#include <memory>
#include "citygml/texture.h"

using namespace plateau::geometry;

Mesh::Mesh(const std::string &id) :
    uv1_(UV()),
    uv2_(UV()),
    uv3_(UV()),
    multiTexture_(MultiTexture()),
    // TODO ここでPolygonに渡すためだけに間に合わせ的に作った shared_ptr がどう動くのかは未検証
    Polygon(id, std::make_shared<PlateauDllLogger>()){
}


void Mesh::setUV2(const UV& uv2) {
    if(uv2.size() != m_vertices.size()){
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

const MultiTexture &Mesh::getMultiTexture() const {
    return multiTexture_;
}

void Mesh::Merge(const Polygon &otherPoly, const TVec2f& UV2Element, const TVec2f& UV3Element) {
    if(otherPoly.getVertices().size() <= 0) return;
    unsigned prevNumVertices = m_vertices.size();
    unsigned prevNumIndices = m_indices.size();
    auto& otherVertices = otherPoly.getVertices();
    auto& otherIndices = otherPoly.getIndices();
    // 頂点リストの末尾に追加します。
    for(const auto & otherVertex : otherVertices){
        m_vertices.push_back(otherVertex);
    }
    // インデックスリストの末尾に追加します。
    for(unsigned int otherIndex : otherIndices){
        m_indices.push_back(otherIndex);
    }
    // 追加分のインデックスを新しい値にします。前の頂点の数だけインデックスの数値を大きくすれば良いです。
    for(unsigned i=prevNumIndices; i<m_indices.size(); i++){
        m_indices.at(i) += prevNumVertices;
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
    if(otherTexture == nullptr){
        otherTexture = Texture::noneTexture;
    }
    bool isDifferentTex = multiTexture_.empty();
    if(!isDifferentTex){
        auto& lastTexture = (*multiTexture_.rbegin()).second;
        if(lastTexture != nullptr){
            isDifferentTex |= otherTexture->getUrl() != lastTexture->getUrl();
        }
    }
    if( isDifferentTex ){
        multiTexture_.emplace(prevNumIndices, otherTexture);
    }
}
