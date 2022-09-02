
#include <vector>
#include "citygml/citymodel.h"
#include "plateau/geometry/geo_reference.h"
#include <plateau/polygon_mesh/mesh_extractor.h>
#include <plateau/polygon_mesh/mesh.h>
#include <plateau/polygon_mesh/mesh_extract_options.h>

namespace plateau::polygonMesh {
    /// グループIDと、その結合後Meshのmapです。
    using GridMergeResult = std::map<unsigned, Mesh>;

    /**
     * cityModel をグリッド状に分割し、各地物オブジェクトをグリッドに分類します。
     * グリッドをさらにグループ分けし、
     * グループごとにメッシュを結合します。
     * 呼び出し元は MeshExtractor::extract 関数から必要な時に利用されます。
     */
    class LIBPLATEAU_EXPORT GridMerger {
    public:
        /**
         * city_model の範囲をグリッド状に分割して、グリッドをさらにグループに分け、各グループ内のメッシュを結合して返します。
         */
        static GridMergeResult
        gridMerge(const citygml::CityModel& city_model, const MeshExtractOptions& options, unsigned lod,
                  const plateau::geometry::GeoReference& geo_reference);
    };
}
