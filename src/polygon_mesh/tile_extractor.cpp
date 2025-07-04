#include "plateau/polygon_mesh/tile_extractor.h"
#include <plateau/polygon_mesh/primary_city_object_types.h>
#include "citygml/texture.h"
#include "citygml/cityobject.h"
#include "plateau/polygon_mesh/map_attacher.h"
#include <plateau/polygon_mesh/mesh_factory.h>
#include <plateau/polygon_mesh/polygon_mesh_utils.h>
#include <plateau/dataset/gml_file.h>
#include <plateau/texture/texture_packer.h>
#include "area_mesh_factory.h"

namespace {
    using namespace plateau;
    using namespace polygonMesh;
    using namespace dataset;
    using namespace texture;
    using namespace citygml;
    namespace fs = std::filesystem;

    /// extentsの幅と奥行きの長さを multiplier 倍にします。
    std::vector<geometry::Extent> extendExtents(const std::vector<geometry::Extent>& src_extents, float multiplier) {
        auto result = std::vector<geometry::Extent>();

        for (const auto& src_extent : src_extents) {
            const auto center = src_extent.centerPoint();
            const auto prev_min = src_extent.min;
            const auto prev_max = src_extent.max;
            auto next_min = center + (prev_min - center) * multiplier;
            auto next_max = center + (prev_max - center) * multiplier;
            result.emplace_back(next_min, next_max);
        }
        return result;
    }

    void extractInner(
        Model& out_model, CityModelVector city_models,
        const MeshExtractOptions& options,
        const std::vector<geometry::Extent>& extents_before_adjust) {

        if (options.max_lod < options.min_lod) throw std::logic_error("Invalid LOD range.");

        const auto geo_reference = geometry::GeoReference(options.coordinate_zone_id, options.reference_point, options.unit_scale, options.mesh_axes);

        // 範囲の境界上にある地物を取り逃さないように、範囲を少し広げます。
        auto extents = extendExtents(extents_before_adjust, 1.2f);

        // rootNode として LODノード を作ります。
        for (unsigned lod = options.min_lod; lod <= options.max_lod; lod++) {
            auto lod_node = Node("LOD" + std::to_string(lod));

            // LODノードの下にメッシュ配置用ノードを作ります。
            {
                // 次のような階層構造を作ります:
                // model -> LODノード -> グループごとのノード

                // 3D都市モデルをグループに分け、グループごとにメッシュをマージします。
				auto result = AreaMeshFactory::combine(city_models, options, lod, geo_reference, extents);

                // グループごとのノードを追加します。
                for (auto& [group_id, mesh] : result) {
                    auto node = Node("group" + std::to_string(group_id), std::move(mesh));
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

        const auto& gmlPath = city_models->empty() || city_models->front().expired() ? "" : city_models->front().lock()->getGmlPath();

        // 現在の都市モデルが地形であるなら、衛星写真または地図用のUVを付与し、地図タイルをダウンロードします。
        auto package = GmlFile(gmlPath).getPackage();
        if (package == PredefinedCityModelPackage::Relief && options.attach_map_tile) {
            const auto gml_path = fs::u8path(gmlPath);
            const auto map_download_dest = gml_path.parent_path() / (gml_path.filename().u8string() + "_map");
            MapAttacher().attach(out_model, options.map_tile_url, map_download_dest, options.map_tile_zoom_level,
                geo_reference);
        }
    }
}

namespace plateau::polygonMesh {

    std::shared_ptr<Model> TileExtractor::extractInExtents(
        CityModelVector city_models, const MeshExtractOptions& options,
        const std::vector<plateau::geometry::Extent>& extents) {
        auto result = std::make_shared<Model>();
        extractInExtents(*result, city_models, options, extents);
        return result;
    }

    void TileExtractor::extractInExtents(
        Model& out_model,
        CityModelVector city_models, const MeshExtractOptions& options,
        const std::vector<plateau::geometry::Extent>& extents) {
        extractInner(out_model, city_models, options, extents);
    }

}
