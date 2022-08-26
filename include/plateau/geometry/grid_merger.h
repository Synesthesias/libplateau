
#include <vector>
#include "mesh.h"
#include "citygml/citymodel.h"
#include "mesh_extract_options.h"
#include "mesh_extractor.h"

namespace plateau::geometry{
    /**
    * グリッド番号と、そのグリッドに属する CityObject のリストを対応付ける辞書です。
    */
    using GridIdToObjsMap = std::map<int, std::list<CityObjectWithImportID>>;
    using GridMergeResult = std::vector<Mesh>;

    /**
     * cityModel をグリッド状に分割し、各地物オブジェクトをグリッドに分類し、
     * グリッドごとにメッシュを結合します。
     * 呼び出し元は MeshExtractor::extract 関数から必要な時に利用されます。
     */
    class LIBPLATEAU_EXPORT GridMerger{
    public:
        /**
         * cityModel の範囲をグリッド状に分割して、グリッド内のメッシュを結合して返します。
         * static関数です。
         */
        [[nodiscard]] static GridMergeResult gridMerge(const CityModel &cityModel, const MeshExtractOptions &options);
    };
}