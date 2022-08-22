#pragma once
#include <libplateau_api.h>
#include <memory>
#include <plateau/geometry/mesh.h>
#include <plateau/geometry/mesh_extract_options.h>
#include "citygml/citymodel.h"

namespace plateau::geometry {
/**
 * @brief
 * GridMerge関数に CityModel を渡すと、配下のジオメトリをすべて検索して グリッド上のポリゴンにまとめます。
 */
    class LIBPLATEAU_EXPORT MeshExtractor {
        using GridMergeResult = std::vector<std::unique_ptr<Mesh>>;
    public:

        /**
         * cityModel の範囲をグリッド状に分割して、グリッド内のメッシュを結合します。
         * 結果は getLastGridMergeResult() で取得できます。
         */
        // TODO これはプロトタイプ実装なので、仕様書に沿った形に書き換える必要があります。
        void
        gridMerge(const CityModel &cityModel, CityObject::CityObjectsType targetTypeMask, int gridNumX, int gridNumY,
                  const std::shared_ptr<PlateauDllLogger> &logger);

        GridMergeResult &getLastGridMergeResult();

    private:
        std::unique_ptr<GridMergeResult> lastGridMergeResult_;

    };

    class CityObjectWithImportID {
    public:
        CityObjectWithImportID(const CityObject *const cityObject, int primaryImportID, int secondaryImportID);

        const CityObject *const getCityObject() { return cityObject; }

        int getPrimaryImportID() { return primaryImportID; }

        int getSecondaryImportID() { return secondaryImportID; }

    private:
        const CityObject *const cityObject;
        int primaryImportID;
        int secondaryImportID;
    };
}