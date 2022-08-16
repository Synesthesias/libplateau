#pragma once
#include <libplateau_api.h>
#include <memory>
#include "plateau_polygon.h"
#include "citygml/citymodel.h"


/**
 * @brief
 * GridMerge関数に CityModel を渡すと、配下のジオメトリをすべて検索して グリッド上のポリゴンにまとめます。
 */
class LIBPLATEAU_EXPORT MeshMerger {
    using GridMergeResult = std::shared_ptr<std::vector<PlateauPolygon*>>;
public:
    /**
     * cityModel の範囲をグリッド状に分割して、グリッド内のメッシュを結合します。
     * 結果は getLastGridMergeResult() で取得できます。
     */
    void gridMerge(const CityModel *cityModel, CityObject::CityObjectsType targetTypeMask, int gridNumX, int gridNumY, const std::shared_ptr<PlateauDllLogger> logger);
    GridMergeResult getLastGridMergeResult();

private:
    GridMergeResult lastGridMergeResult_;
};
