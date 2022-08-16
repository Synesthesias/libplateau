
#include <plateau/io/plateau_polygon.h>

PlateauPolygon::PlateauPolygon(const std::string& id, std::shared_ptr<PlateauDllLogger> logger) :
    Polygon(id, logger),
    logger_(logger){
}


void PlateauPolygon::setUV2(std::unique_ptr<UV2> uv2) {
    if(uv2->size() != m_vertices.size()){
        logger_->throwException("Size of uv2 does not match num of vertices.");
        return;
    }
    uv2_ = std::move(uv2);
}

const UV2& PlateauPolygon::getUV2(){
    return *uv2_;
}

void PlateauPolygon::Merge(const Polygon &otherPoly) {
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
}
