#pragma once
#include <plateau/polygon_mesh/model.h>

namespace plateau::granularityConvert {
    class IModelConverter {
    public:
        virtual plateau::polygonMesh::Model convert(plateau::polygonMesh::Model* src) = 0;
    };
}
