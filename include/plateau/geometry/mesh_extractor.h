#pragma once
#include <libplateau_api.h>
#include <memory>
#include <plateau/geometry/mesh.h>
#include <plateau/geometry/mesh_extract_options.h>
#include "citygml/citymodel.h"
#include "model.h"

namespace plateau::geometry {
/**
 * @brief
 * CityModelからModel(メッシュ等)を取り出します。
 * GridMerge関数に CityModel を渡すと、配下のジオメトリをすべて検索して グリッド上のポリゴンにまとめます。
 */
    class LIBPLATEAU_EXPORT MeshExtractor {
        using GridMergeResult = std::vector<Mesh>;
    public:

        std::shared_ptr<Model> extract(const CityModel& cityModel, MeshExtractOptions options, const std::shared_ptr<PlateauDllLogger> &logger);

        /**
         * cityModel の範囲をグリッド状に分割して、グリッド内のメッシュを結合します。
         * 結果は getLastGridMergeResult() で取得できます。
         */
        // TODO 仕様変更に対応する。グリッドマージはextractの機能の一部になるのでprivateで良いはず。
        void
        gridMerge(const CityModel &cityModel, const MeshExtractOptions &options,
                  const std::shared_ptr<PlateauDllLogger> &logger);

        GridMergeResult &getLastGridMergeResult();

    private:
        // TODO あとで消す。グリッドマージの結果はここで保存せず直接返すようにする。
        GridMergeResult lastGridMergeResult_;

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