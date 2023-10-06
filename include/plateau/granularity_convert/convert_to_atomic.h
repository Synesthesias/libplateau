#pragma once
#include "i_model_converter.h"

namespace plateau::granularityConvert {

    /// モデルを最小地物単位に変換します。引数の結合単位は問いません。
    class ConvertToAtomic : public IModelConverter {
    public:
        plateau::polygonMesh::Model convert(plateau::polygonMesh::Model* src) override;
    };
}
