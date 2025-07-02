#pragma once

#include <plateau/polygon_mesh/mesh_extractor.h>
#include <libplateau_api.h>
#include <memory>
#include <plateau/polygon_mesh/mesh.h>
#include <plateau/geometry/geo_coordinate.h>
#include "citygml/citymodel.h"
#include "model.h"

namespace plateau::polygonMesh {

    /**
     * CityModelからModel(メッシュ等)を構築します。
     * このクラスの利用者である各ゲームエンジンは、このクラスから受け取った Model を元に
     * ゲームオブジェクト、メッシュ、テクスチャを生成することが期待されます。
     *
     * 詳しくは Model クラスのコメントを参照してください。
     */
    class LIBPLATEAU_EXPORT TileExtractor : MeshExtractor {
    public:

        static std::shared_ptr<Model> extractInExtents(
            std::shared_ptr<std::vector<std::shared_ptr<const citygml::CityModel>>> city_models, const MeshExtractOptions& options,
            const std::vector<plateau::geometry::Extent>& extents);

        static void extractInExtents(Model& out_model,
            std::shared_ptr<std::vector<std::shared_ptr<const citygml::CityModel>>> city_models, const MeshExtractOptions& options,
            const std::vector<plateau::geometry::Extent>& extents);
    };
}
