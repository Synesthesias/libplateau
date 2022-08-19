
#include <plateau/io/plateau_polygon.h>
#include "citygml/texture.h"

PlateauPolygon::PlateauPolygon(const std::string& id, std::shared_ptr<PlateauDllLogger> logger) :
    Polygon(id, logger),
    uv1_(std::make_unique<UV>()),
    uv2_(std::make_unique<UV>()),
    multiTexture_(std::make_unique<MultiTexture>()),
    logger_(logger){
}


void PlateauPolygon::setUV2(std::unique_ptr<UV> uv2) {
    if(uv2->size() != m_vertices.size()){
        logger_->throwException("Size of uv2 does not match num of vertices.");
        return;
    }
    uv2_ = std::move(uv2);
}

const UV& PlateauPolygon::getUV1() const{
    return *uv1_;
}

const UV& PlateauPolygon::getUV2() const{
    return *uv2_;
}

const MultiTexture &PlateauPolygon::getMultiTexture() const {
    return *(multiTexture_);
}

void PlateauPolygon::Merge(const Polygon &otherPoly) {
    if(otherPoly.getVertices().size() <= 0) return;
    int prevNumVertices = m_vertices.size();
    int prevNumIndices = m_indices.size();
    auto& otherVertices = otherPoly.getVertices();
    auto& otherIndices = otherPoly.getIndices();
    // 頂点リストの末尾に追加します。
    m_vertices.insert(m_vertices.end(), otherVertices.begin(), otherVertices.end());
    // インデックスリストの末尾に追加します。
    m_indices.insert(m_indices.end(), otherIndices.begin(), otherIndices.end());
    // 追加分のインデックスを新しい値にします。前の頂点の数だけインデックスの数値を大きくすれば良いです。
    for(int i=prevNumIndices; i<m_indices.size(); i++){
        m_indices.at(i) += prevNumVertices;
    }
    // UV1を追加します。
    auto otherUV1 = otherPoly.getTexCoordsForTheme("rgbTexture", true);
    // otherUV1 の数が頂点数に足りなければ 0 で埋めます
    for(size_t i = otherUV1.size(); i < otherVertices.size(); i++){
        otherUV1.emplace_back(0,0);
    }
    uv1_->insert(uv1_->end(), otherUV1.begin(), otherUV1.end());
    // テクスチャが異なる場合は追加します。
    // TODO ここのコードは整理したい
    auto otherTexture = otherPoly.getTextureFor("rgbTexture");
    bool isDifferentTex = multiTexture_->size() <= 0;
    if(!isDifferentTex){
        auto& lastTexture = (*multiTexture_->rbegin()).second;
        if(lastTexture != nullptr && otherTexture != nullptr){
            isDifferentTex |= otherTexture->getUrl() != lastTexture->getUrl();
        }
    }
    if(otherTexture == nullptr){
        otherTexture = Texture::noneTexture;
    }
    if( isDifferentTex ){
        multiTexture_->emplace(prevNumIndices, otherTexture);
    }
}
