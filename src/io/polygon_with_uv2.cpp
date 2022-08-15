
#include <plateau/io/polygon_with_uv2.h>

PolygonWithUV2::PolygonWithUV2(const std::string& id, std::shared_ptr<PlateauDllLogger> logger) :
    Polygon(id, logger),
    logger_(logger){
}


void PolygonWithUV2::setUV2(std::unique_ptr<UV2> uv2) {
    if(uv2->size() != m_vertices.size()){
        logger_->throwException("Size of uv2 does not match num of vertices.");
        return;
    }
    uv2_ = std::move(uv2);
}

const UV2& PolygonWithUV2::getUV2(){
    return *uv2_;
}

/**
 * 頂点リストとインデックスリストをマージします。
 */
void PolygonWithUV2::Merge(const Polygon &otherPoly) {
    int prevNumVertices = m_vertices.size();
    auto& otherVertices = otherPoly.getVertices();
    auto& otherIndices = otherPoly.getIndices();
    m_vertices.insert(m_vertices.end(), otherVertices.begin(), otherVertices.end());
    m_indices.insert(m_indices.end(), otherIndices.begin(), otherIndices.end());
    for(int i=prevNumVertices; i<m_indices.size(); i++){
        m_indices.at(i) += prevNumVertices;
    }
}
