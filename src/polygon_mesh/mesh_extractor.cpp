#include <plateau/polygon_mesh/mesh_extractor.h>
#include <plateau/polygon_mesh/primary_city_object_types.h>
#include "citygml/texture.h"
#include "../src/polygon_mesh/grid_merger.h"
#include "citygml/cityobject.h"
#include <plateau/polygon_mesh/mesh_merger.h>
#include <plateau/polygon_mesh/polygon_mesh_utils.h>

namespace {
    using namespace plateau;
    using namespace polygonMesh;

    bool shouldSkipCityObj(const citygml::CityObject& city_obj, const MeshExtractOptions& options) {
        return options.exclude_city_object_outside_extent && !options.extent.contains(city_obj);
    }

    void extractInner(
        Model& out_model, const citygml::CityModel& city_model,
        const MeshExtractOptions& options) {

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
                auto result = AreaMeshFactory::gridMerge(city_model, options, lod, geo_reference);
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
                    if (shouldSkipCityObj(*primary_object, options))
                        continue;

                    // 主要地物のメッシュを作ります。
                    MeshFactory mesh_factory(nullptr, options, geo_reference);
                    
                    if (MeshExtractor::shouldContainPrimaryMesh(lod, *primary_object)) {
                        mesh_factory.addPolygonsInPrimaryCityObject(*primary_object, lod, city_model.getGmlPath());
                    }

                    if (lod >= 2) {
                        // 主要地物の子である各最小地物をメッシュに加えます。
                        auto atomic_objects = PolygonMeshUtils::getChildCityObjectsRecursive(*primary_object);
                        mesh_factory.addPolygonsInAtomicCityObjects(*primary_object, atomic_objects, lod, city_model.getGmlPath());
                    }

                    // 主要地物ごとのノードを追加します。
                    lod_node.addChildNode(Node(primary_object->getId(), mesh_factory.releaseMesh()));
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
                    if (shouldSkipCityObj(*primary_city_object, options))
                        continue;

                    // 主要地物のノードを作成します。
                    std::unique_ptr<Mesh> primary_mesh;
                    if (MeshExtractor::shouldContainPrimaryMesh(lod, *primary_city_object)) {
                        MeshFactory primary_mesh_factory(nullptr, options, geo_reference);
                        primary_mesh_factory.addPolygonsInPrimaryCityObject(*primary_city_object, lod, city_model.getGmlPath());
                        primary_mesh = primary_mesh_factory.releaseMesh();
                    }
                    auto primary_node = Node(primary_city_object->getId(), std::move(primary_mesh));

                    // 最小地物ごとにノードを作成
                    auto atomic_objects = PolygonMeshUtils::getChildCityObjectsRecursive(*primary_city_object);
                    for (auto atomic_object : atomic_objects) {
                        MeshFactory atomic_mesh_factory(nullptr, options, geo_reference);
                        atomic_mesh_factory.addPolygonsInAtomicCityObject(
                            *primary_city_object, *atomic_object,
                            lod, city_model.getGmlPath());
                        auto atomic_node = Node(atomic_object->getId(), atomic_mesh_factory.releaseMesh());
                        primary_node.addChildNode(std::move(atomic_node));
                    }
                    lod_node.addChildNode(std::move(primary_node));
                }

            }
            break;
            default:
                throw std::logic_error("Unknown enum type of options.mesh_granularity .");
            }

            out_model.addNode(std::move(lod_node));
        }
        out_model.eraseEmptyNodes();
    }
}

namespace plateau::polygonMesh {
    std::shared_ptr<Model> MeshExtractor::extract(const citygml::CityModel& city_model,
                                                  const MeshExtractOptions& options) {
        auto result = std::make_shared<Model>();
        extract(*result, city_model, options);
        return result;
    }

    void MeshExtractor::extract(Model& out_model, const citygml::CityModel& city_model,
                                const MeshExtractOptions& options) {
        extractInner(out_model, city_model, options);
    }

    bool MeshExtractor::shouldContainPrimaryMesh(unsigned lod, const citygml::CityObject& primary_obj) {
        // LOD2以上の建築物以外の場合主要地物の形状を含める
        return !(lod >= 2 &&
                 (primary_obj.getType() & citygml::CityObject::CityObjectsType::COT_Building) !=
                 static_cast<citygml::CityObject::CityObjectsType>(0));
    }
}
