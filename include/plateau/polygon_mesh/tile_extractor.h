#pragma once

#include <plateau/polygon_mesh/mesh_extractor.h>
#include <plateau/polygon_mesh/polygon_mesh_types.h>
#include <libplateau_api.h>
#include <memory>
#include "citygml/citymodel.h"
#include "model.h"

namespace plateau::polygonMesh {

    /**
	 * TileExtractorは、複数のCityModelからメッシュを抽出し、指定された範囲(extents)に基づいて結合メッシュを抽出するクラスです。
     * 
     */
    class LIBPLATEAU_EXPORT TileExtractor : public MeshExtractor {
    public:

        /**
         * CityModelのリストを結合し範囲内のModelを取り出します。
         */
        static std::shared_ptr<Model> extractWithCombine(
            const CityModelVector& city_models, const MeshExtractOptions& options,
            const std::vector<plateau::geometry::Extent>& extents);

        /**
         * CityModelのリストを結合し範囲内のModelを取り出します。
         */
        static void extractWithCombine(Model& out_model,
            const CityModelVector& city_models, const MeshExtractOptions& options,
            const std::vector<plateau::geometry::Extent>& extents);

        /**
         * CityModelから範囲内のModelを取り出しグリッド分割します。
         */
        static std::shared_ptr<Model> extractWithGrid(
            const citygml::CityModel& city_model, const MeshExtractOptions& options, 
            const std::vector<plateau::geometry::Extent>& extents);

        /**
         * CityModelから範囲内のModelを取り出しグリッド分割します。
         */
        static void extractWithGrid(Model& out_model,
            const citygml::CityModel& city_model, const MeshExtractOptions& options, 
            const std::vector<plateau::geometry::Extent>& extents);
    };
}
