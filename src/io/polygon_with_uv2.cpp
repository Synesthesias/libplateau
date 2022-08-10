
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