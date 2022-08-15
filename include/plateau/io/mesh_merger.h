#pragma once
#include <libplateau_api.h>
#include <memory>
#include "polygon_with_uv2.h"
#include "citygml/citymodel.h"


/**
 * @brief
 * GridMerge関数に CityModel を渡すと、配下のジオメトリをすべて検索して グリッド上のポリゴンにまとめます。
 */
class LIBPLATEAU_EXPORT MeshMerger {
public:
    static /*std::vector<PolygonWithUV2>*/ void GridMerge(const CityModel &cityModel, CityObject::CityObjectsType targetTypeMask, int gridNumX, int gridNumY, std::shared_ptr<PlateauDllLogger> logger);
};
