#include <plateau/polygon_mesh/mesh_extractor.h>
#include <plateau/polygon_mesh/primary_city_object_types.h>
#include "citygml/texture.h"
#include "area_mesh_factory.h"
#include "citygml/cityobject.h"
#include "plateau/polygon_mesh/map_attacher.h"
#include <plateau/polygon_mesh/mesh_factory.h>
#include <plateau/polygon_mesh/polygon_mesh_utils.h>
#include <plateau/dataset/gml_file.h>
#include <plateau/texture/texture_packer.h>

namespace {
    using namespace plateau;
    using namespace polygonMesh;
    using namespace dataset;
    using namespace texture;
    using namespace citygml;
    namespace fs = std::filesystem;

    bool shouldSkipCityObj(const CityObject& city_obj, const MeshExtractOptions& options, const std::vector<geometry::Extent>& extents) {

        // 範囲内であっても、COT_Roomは意図的に省きます。なぜなら、LOD4の建物においてRoomと天井、床等が完全に重複するのをなくしたいからです。
        if(city_obj.getType() == CityObject::CityObjectsType::COT_Room) return true;

        // 範囲外を省く設定ならば省きます。
        if (!options.exclude_city_object_outside_extent)
            return false;

        for (const auto& extent : extents) {
            if (extent.contains(city_obj))
                return false;
        }

        return true;
    }

    void extractInner(
        Model& out_model, const CityModel& city_model,
        const MeshExtractOptions& options,
        const std::vector<geometry::Extent>& extents) {

        if (options.max_lod < options.min_lod) throw std::logic_error("Invalid LOD range.");

        const auto geo_reference = geometry::GeoReference(options.coordinate_zone_id, options.reference_point, options.unit_scale, options.mesh_axes);

        // rootNode として LODノード を作ります。
        for (unsigned lod = options.min_lod; lod <= options.max_lod; lod++) {
            auto lod_node = Node("LOD" + std::to_string(lod));

            // LODノードの下にメッシュ配置用ノードを作ります。
            switch (options.mesh_granularity) {
            case MeshGranularity::PerCityModelArea:
            {
                // 次のような階層構造を作ります:
                // model -> LODノード -> グループごとのノード

                // 3D都市モデルをグループに分け、グループごとにメッシュをマージします。
                auto result = AreaMeshFactory::gridMerge(city_model, options, lod, geo_reference, extents);
                // グループごとのノードを追加します。
                for (auto& [group_id, mesh] : result) {
                    auto node = Node("group" + std::to_string(group_id), std::move(mesh));
                    lod_node.addChildNode(std::move(node));
                }
            }
            break;
            case MeshGranularity::PerPrimaryFeatureObject:
            {
                // 次のような階層構造を作ります：
                // model -> LODノード -> 主要地物ごとのノード

                auto& all_primary_city_objects_in_model =
                    city_model.getAllCityObjectsOfType(PrimaryCityObjectTypes::getPrimaryTypeMask());

                // 主要地物ごとにメッシュを結合します。
                for (auto primary_object : all_primary_city_objects_in_model) {
                    // 範囲外ならスキップします。
                    if (shouldSkipCityObj(*primary_object, options, extents))
                        continue;
                    if (MeshExtractor::isTypeToSkip(primary_object->getType())) continue;

                    // 主要地物のメッシュを作ります。
                    MeshFactory mesh_factory(nullptr, options, extents, geo_reference);
                    
                    if (MeshExtractor::shouldContainPrimaryMesh(lod, *primary_object)) {
                        mesh_factory.addPolygonsInPrimaryCityObject(*primary_object, lod, city_model.getGmlPath());
                    }

                    if (lod >= 2) {
                        // 主要地物の子である各最小地物をメッシュに加えます。
                        auto atomic_objects = PolygonMeshUtils::getChildCityObjectsRecursive(*primary_object);
                        mesh_factory.addPolygonsInAtomicCityObjects(*primary_object, atomic_objects, lod, city_model.getGmlPath());
                    }

                    // 主要地物ごとのノードを追加します。
                    mesh_factory.optimizeMesh();
                    lod_node.addChildNode(Node(primary_object->getId(), mesh_factory.releaseMesh()));
                    mesh_factory.incrementPrimaryIndex();
                }
            }
            break;
            case MeshGranularity::PerAtomicFeatureObject:
            {
                // 次のような階層構造を作ります：
                // model -> LODノード -> 主要地物ごとのノード -> その子の最小地物ごとのノード
                auto& primary_city_objects = city_model.getAllCityObjectsOfType(
                        PrimaryCityObjectTypes::getPrimaryTypeMask());
                for (auto primary_city_object : primary_city_objects) {
                    // 範囲外ならスキップします。
                    if (shouldSkipCityObj(*primary_city_object, options, extents))
                        continue;
                    if (MeshExtractor::isTypeToSkip(primary_city_object->getType())) continue;

                    // 主要地物のノードを作成します。
                    std::unique_ptr<Mesh> primary_mesh;
                    MeshFactory primary_mesh_factory(nullptr, options, extents, geo_reference);
                    if (MeshExtractor::shouldContainPrimaryMesh(lod, *primary_city_object)) {
                        primary_mesh_factory.addPolygonsInPrimaryCityObject(*primary_city_object, lod, city_model.getGmlPath());
                        primary_mesh = primary_mesh_factory.releaseMesh();
                    }
                    auto primary_node = Node(primary_city_object->getId(), std::move(primary_mesh));

                    // 最小地物ごとにノードを作成
                    auto atomic_objects = PolygonMeshUtils::getChildCityObjectsRecursive(*primary_city_object);
                    for (auto atomic_object : atomic_objects) {
                        if(MeshExtractor::isTypeToSkip(atomic_object->getType())) continue;
                        MeshFactory atomic_mesh_factory(nullptr, options, extents, geo_reference);
                        atomic_mesh_factory.addPolygonsInAtomicCityObject(
                            *primary_city_object, *atomic_object,
                            lod, city_model.getGmlPath());
                        auto atomic_node = Node(atomic_object->getId(), atomic_mesh_factory.releaseMesh());
                        primary_node.addChildNode(std::move(atomic_node));
                    }
                    lod_node.addChildNode(std::move(primary_node));
                    primary_mesh_factory.incrementPrimaryIndex();
                }
            }
            break;
            default:
                throw std::logic_error("Unknown enum type of options.mesh_granularity .");
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

        // 現在の都市モデルが地形であるなら、衛星写真または地図用のUVを付与し、地図タイルをダウンロードします。
        auto package = GmlFile(city_model.getGmlPath()).getPackage();
        if(package == PredefinedCityModelPackage::Relief && options.attach_map_tile) {
            const auto gml_path = fs::u8path(city_model.getGmlPath());
            const auto map_download_dest = gml_path.parent_path() / (gml_path.filename().u8string() + "_map");
            MapAttacher().attach(out_model, options.map_tile_url, map_download_dest, options.map_tile_zoom_level,
                                geo_reference);
        }
    }
}

namespace plateau::polygonMesh {
    std::shared_ptr<Model> MeshExtractor::extract(const CityModel& city_model,
                                                  const MeshExtractOptions& options) {
        auto result = std::make_shared<Model>();
        extract(*result, city_model, options);
        return result;
    }

    void MeshExtractor::extract(Model& out_model, const CityModel& city_model,
                                const MeshExtractOptions& options) {
        extractInner(out_model, city_model, options, { plateau::geometry::Extent::all() });
    }

    std::shared_ptr<Model> MeshExtractor::extractInExtents(
        const CityModel& city_model, const MeshExtractOptions& options,
        const std::vector<plateau::geometry::Extent>& extents) {

        auto result = std::make_shared<Model>();
        extractInExtents(*result, city_model, options, extents);
        return result;
    }

    void MeshExtractor::extractInExtents(
        Model& out_model, const CityModel& city_model,
        const MeshExtractOptions& options,
        const std::vector<plateau::geometry::Extent>& extents) {


        extractInner(out_model, city_model, options, extents);
    }



    bool MeshExtractor::shouldContainPrimaryMesh(unsigned lod, const CityObject& primary_obj) {
        // LOD2以上の建築物の場合、重複を防ぐため主要地物を含めません。
        bool is_building_lod_gte2 =
                lod >= 2 &&
                (primary_obj.getType() & CityObject::CityObjectsType::COT_Building) != static_cast<CityObject::CityObjectsType>(0);
        // LOD2以上の道路の場合も同様に主要地物を含めません。
        bool is_road_lod_gte2 =
                lod >= 2 &&
                (primary_obj.getType() & CityObject::CityObjectsType::COT_Road) != static_cast<CityObject::CityObjectsType>(0);
        return !is_building_lod_gte2 && !is_road_lod_gte2;
    }

    bool MeshExtractor::isTypeToSkip(CityObject::CityObjectsType type) {

        return
            // COT_Roomは省きます。なぜなら、LOD4の建物においてRoomと天井、床等が完全に重複するのをなくしたいからです。
            type == CityObject::CityObjectsType::COT_Room ||
            // COT_CityObjectGroupも省きます。なぜなら、LOD4の建物でbldg以下の建物パーツと重複するのをなくしたいからです。
            type == CityObject::CityObjectsType::COT_CityObjectGroup;
    }
}
