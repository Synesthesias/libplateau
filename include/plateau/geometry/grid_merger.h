
#include <vector>
#include "mesh.h"
#include "citygml/citymodel.h"
#include "mesh_extract_options.h"
#include "mesh_extractor.h"

namespace plateau::geometry{
    /**
    * グリッド番号と、そのグリッドに属する CityObject のリストを対応付ける辞書です。
    */
    using GridIDToObjsMap = std::map<unsigned, std::list<CityObjectWithImportID>>;
    using GroupIDToObjsMap = GridIDToObjsMap;
    /// グループIDと、その結合後Meshのmapです。
    using GridMergeResult = std::map<unsigned, Mesh>;

    /**
     * cityModel をグリッド状に分割し、各地物オブジェクトをグリッドに分類します。
     * グリッドをさらにグループ分けし、
     * グループごとにメッシュを結合します。
     * 呼び出し元は MeshExtractor::extract 関数から必要な時に利用されます。
     */
    class LIBPLATEAU_EXPORT GridMerger{
    public:
        /**
         * cityModel の範囲をグリッド状に分割して、グリッドをさらにグループに分け、各グループ内のメッシュを結合して返します。
         */
        [[nodiscard]] static GridMergeResult gridMerge(const CityModel &cityModel, const MeshExtractOptions &options, unsigned LOD);
    };
}