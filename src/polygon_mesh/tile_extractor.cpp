#include <plateau/polygon_mesh/tile_extractor.h>
#include <plateau/polygon_mesh/map_attacher.h>
#include <plateau/polygon_mesh/mesh_factory.h>
#include <plateau/polygon_mesh/polygon_mesh_utils.h>
#include <plateau/polygon_mesh/primary_city_object_types.h>
#include <plateau/dataset/gml_file.h>
#include <plateau/texture/texture_packer.h>
#include <citygml/cityobject.h>
#include <citygml/texture.h>
#include "area_mesh_factory.h"
#include <filesystem>

namespace {
    using namespace plateau;
    using namespace polygonMesh;
    using namespace dataset;
    using namespace texture;
    using namespace citygml;
    namespace fs = std::filesystem;

    constexpr float EXTENT_EXPANSION_FACTOR = 1.2f;

    Node& findOrCreateGridNode(Model& model, const std::string& grid_name) {
        const auto grid_index = model.getRootNodeIndexByName(grid_name);
        if (grid_index == -1) {
            auto grid_node = Node(grid_name);
            return model.addNode(std::move(grid_node));
        }
        return model.getRootNodeAt(grid_index);
    }

    Node& findOrCreateLodNode(Node& parent_node, const std::string& lod_name) {
        const auto lod_index = parent_node.getChildIndexByName(lod_name);
        if (lod_index == -1) {
            auto lod_node = Node(lod_name);
            return parent_node.addChildNode(std::move(lod_node));
        }
        return parent_node.getChildAt(lod_index);
    }

    void extractWithCombineInner(
        Model& out_model, CityModelVector city_models,
        const MeshExtractOptions& options,
        const std::vector<geometry::Extent>& extents_before_adjust) {

        if (options.max_lod < options.min_lod) throw std::logic_error("Invalid LOD range.");

        const auto geo_reference = geometry::GeoReference(options.coordinate_zone_id, options.reference_point, options.unit_scale, options.mesh_axes);

        // 範囲の境界上にある地物を取り逃さないように、範囲を少し広げます。
        auto extents = MeshExtractor::extendExtents(extents_before_adjust, EXTENT_EXPANSION_FACTOR);

        // rootNode として LODノード を作ります。
        for (int lod = options.min_lod; lod <= options.max_lod; lod++) {
            auto lod_node = Node("LOD" + std::to_string(lod));

            // LODノードの下にメッシュ配置用ノードを作ります。
            {
                if (!city_models || city_models->empty()) {
                        // 入力なしの場合はこの LOD をスキップ
                        continue;    
                }
                // 次のような階層構造を作ります:
                // model -> LODノード -> ノード

                // 3D都市モデルリストのメッシュを全てマージします。
				auto result = AreaMeshFactory::combine(city_models, options, lod, geo_reference, extents);

                // グループごとのノードを追加します。
                for (auto& [group_grid_id, mesh] : result) {
                    auto node = Node("group" + std::to_string(group_grid_id.first), std::move(mesh));
                    lod_node.addChildNode(std::move(node));
                }
            }
            out_model.addNode(std::move(lod_node));
        }
        out_model.eraseEmptyNodes();
        out_model.assignNodeHierarchy();

        // テクスチャを結合します。
        if (options.enable_texture_packing) {
            TexturePacker packer(options.texture_packing_resolution, options.texture_packing_resolution);
            packer.process(out_model);
        }

        const auto & gmlPath = (!city_models || city_models->empty() || city_models->front().expired()) ? "" : city_models->front().lock()->getGmlPath();
        
        // 現在の都市モデルが地形であるなら、衛星写真または地図用のUVを付与し、地図タイルをダウンロードします。
        if (!gmlPath.empty()) {
            auto package = GmlFile(gmlPath).getPackage();
            if (package == PredefinedCityModelPackage::Relief && options.attach_map_tile) {
                const auto gml_path = fs::u8path(gmlPath);
                const auto map_download_dest = gml_path.parent_path() / (gml_path.filename().u8string() + "_map");
                MapAttacher().attach(out_model, options.map_tile_url, map_download_dest, options.map_tile_zoom_level, geo_reference);
            }
        }
    }

    void extractWithGridInner(
        Model& out_model, const CityModel& city_model,
        const MeshExtractOptions& options,
        const std::vector<geometry::Extent>& extents_before_adjust) {

        if (options.max_lod < options.min_lod) throw std::logic_error("Invalid LOD range.");

        const auto geo_reference = geometry::GeoReference(options.coordinate_zone_id, options.reference_point, options.unit_scale, options.mesh_axes);

        // 範囲の境界上にある地物を取り逃さないように、範囲を少し広げます。
        auto extents = MeshExtractor::extendExtents(extents_before_adjust, EXTENT_EXPANSION_FACTOR);

        for (int lod = options.min_lod; lod <= options.max_lod; lod++) {            
            // 3D都市モデルをグリッドに分け、グリッドごとにメッシュをマージします。
            auto result = AreaMeshFactory::gridMerge(city_model, options, lod, geo_reference, extents);

            if (options.grid_count_of_side > 1) {
                // 次のような階層構造を作ります:
                // model -> GRIDノード -> LODノード -> ノード
                for (auto& [group_grid_id, mesh] : result) {

					const std::string grid_name = "GRID" + std::to_string(group_grid_id.second + 1); // 1,2,3,4
                    const std::string lod_name = "LOD" + std::to_string(lod);
                    auto node = Node("group" + std::to_string(group_grid_id.first), std::move(mesh));

                    const auto grid_index = out_model.getRootNodeIndexByName(grid_name);
                    auto & grid_node = findOrCreateGridNode(out_model, grid_name);
                    auto & lod_node = findOrCreateLodNode(grid_node, lod_name);
                    lod_node.addChildNode(std::move(node));
                }
            }
            else {
                // 次のような階層構造を作ります:
                // model -> LODノード -> ノード
                auto lod_node = Node("LOD" + std::to_string(lod));
                for (auto& [group_grid_id, mesh] : result) {
                    auto node = Node("group" + std::to_string(group_grid_id.first), std::move(mesh));
                    lod_node.addChildNode(std::move(node));
                }
                out_model.addNode(std::move(lod_node));
            }  
        }

        out_model.eraseEmptyNodes();
        out_model.assignNodeHierarchy();

        // テクスチャを結合します。
        if (options.enable_texture_packing) {
            TexturePacker packer(options.texture_packing_resolution, options.texture_packing_resolution);
            packer.process(out_model);
        }

        // 現在の都市モデルが地形であるなら、衛星写真または地図用のUVを付与し、地図タイルをダウンロードします。
        const auto & gmlPath = city_model.getGmlPath();
        if (!gmlPath.empty()) {
            auto package = GmlFile(gmlPath).getPackage();
            if (package == PredefinedCityModelPackage::Relief && options.attach_map_tile) {
                const auto gml_path = fs::u8path(city_model.getGmlPath());
                const auto map_download_dest = gml_path.parent_path() / (gml_path.filename().u8string() + "_map");
                MapAttacher().attach(out_model, options.map_tile_url, map_download_dest, options.map_tile_zoom_level,
                    geo_reference);
            }
        }
    }
}

namespace plateau::polygonMesh {

    std::shared_ptr<Model> TileExtractor::extractWithCombine(
        const CityModelVector& city_models, const MeshExtractOptions& options,
        const std::vector<plateau::geometry::Extent>& extents) {
        auto result = std::make_shared<Model>();
        extractWithCombine(*result, city_models, options, extents);
        return result;
    }

    void TileExtractor::extractWithCombine(
        Model& out_model,
        const CityModelVector& city_models, const MeshExtractOptions& options,
        const std::vector<plateau::geometry::Extent>& extents) {
        extractWithCombineInner(out_model, city_models, options, extents);
    }

    std::shared_ptr<Model> TileExtractor::extractWithGrid(
        const citygml::CityModel& city_model, const MeshExtractOptions& options,
        const std::vector<plateau::geometry::Extent>& extents) {
        auto result = std::make_shared<Model>();
        extractWithGrid(*result, city_model, options, extents);
        return result;
    }

    void TileExtractor::extractWithGrid(
        Model& out_model,
        const citygml::CityModel& city_model, const MeshExtractOptions& options,
        const std::vector<plateau::geometry::Extent>& extents) {

        if (!options.highest_lod_only) {
            MeshExtractor::extractInExtents(out_model, city_model, options, extents);
        }
        else {
			// 高さLODのみを抽出する場合、グリッド抽出を行います。
            extractWithGridInner(out_model, city_model, options, extents);
        }
    }

}
