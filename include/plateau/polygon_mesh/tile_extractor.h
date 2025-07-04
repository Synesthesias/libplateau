#pragma once

#include <plateau/polygon_mesh/mesh_extractor.h>
#include <libplateau_api.h>
#include <memory>
#include <plateau/polygon_mesh/mesh.h>
#include <plateau/geometry/geo_coordinate.h>
#include "citygml/citymodel.h"
#include "model.h"

namespace plateau::polygonMesh {

    using CityModelVector = std::shared_ptr<std::vector<std::weak_ptr<const citygml::CityModel>>>;

    /**
	 * TileExtractorは、複数のCityModelからメッシュを抽出し、指定された範囲(extents)に基づいて結合メッシュを抽出するクラスです。
     * 
     */
    class LIBPLATEAU_EXPORT TileExtractor : MeshExtractor {
    public:

        static std::shared_ptr<Model> extractInExtents(
            CityModelVector city_models, const MeshExtractOptions& options,
            const std::vector<plateau::geometry::Extent>& extents);

        static void extractInExtents(Model& out_model,
            CityModelVector city_models, const MeshExtractOptions& options,
            const std::vector<plateau::geometry::Extent>& extents);
    };
}
