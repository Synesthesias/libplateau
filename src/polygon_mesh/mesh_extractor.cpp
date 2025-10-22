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
#include <chrono>

#include "plateau_dll_logger.h"

namespace {
    using namespace plateau;
    using namespace polygonMesh;
    using namespace dataset;
    using namespace texture;
    using namespace citygml;
    namespace fs = std::filesystem;

    class Stopwatch {
    public:
        Stopwatch(std::shared_ptr<citygml::CityGMLLogger> logger, const std::string& task_name)
            : logger_(std::move(logger)), task_name_(task_name) {
            total_start_time_ = std::chrono::steady_clock::now();
            stage_start_time_ = total_start_time_;
            logger_->log(citygml::CityGMLLogger::LOGLEVEL::LL_INFO, task_name_ + " start.");
        }

        ~Stopwatch() {
            const auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - total_start_time_).count();
            logger_->log(citygml::CityGMLLogger::LOGLEVEL::LL_INFO, task_name_ + " finished. Total: " + std::to_string(total_duration) + "ms");
        }

        void log_stage(const std::string& stage_name) {
            const auto end_time = std::chrono::steady_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - stage_start_time_).count();
            logger_->log(citygml::CityGMLLogger::LOGLEVEL::LL_INFO, "  " + stage_name + " : " + std::to_string(duration) + "ms");
            stage_start_time_ = end_time;
        }

    private:
        std::shared_ptr<citygml::CityGMLLogger> logger_;
        std::string task_name_;
        std::chrono::steady_clock::time_point total_start_time_;
        std::chrono::steady_clock::time_point stage_start_time_;
    };

    bool shouldSkipCityObj(const CityObject& city_obj, const MeshExtractOptions& options, const std::vector<geometry::Extent>& extents) {

        // 範囲内であっても、COT_Roomは意図的に省きます。なぜなら、LOD4の建物においてRoomと天井、床等が完全に重複するのをなくしたいからです。
        if(city_obj.getType() == CityObject::CityObjectsType::COT_Room) return true;

        // 範囲外を省く設定ならば省きます。
        if (!options.exclude_city_object_outside_extent)
            return false;
        
        for (const auto& extent : extents) {
            if (extent.containsInPolar(city_obj, options.epsg_code))
                return false;
        }
        return true;
    }

    void extractInner(
        Model& out_model, const CityModel& city_model,
        const MeshExtractOptions& options,
        const std::vector<geometry::Extent>& extents_before_adjust,
        std::shared_ptr<citygml::CityGMLLogger> logger) {

        if (logger == nullptr) {
            logger = std::make_shared<PlateauDllLogger>();
        }

        Stopwatch stopwatch(logger, "MeshExtractor");

        if (options.max_lod < options.min_lod) {
            logger->log(citygml::CityGMLLogger::LOGLEVEL::LL_ERROR, "Invalid LOD range.");
            throw std::logic_error("Invalid LOD range.");
        }

        const auto geo_reference = geometry::GeoReference(options.coordinate_zone_id, options.reference_point, options.unit_scale, options.mesh_axes);

        // 範囲の境界上にある地物を取り逃さないように、範囲を少し広げます。
        auto extents = MeshExtractor::extendExtents(extents_before_adjust, 1.2f);
        stopwatch.log_stage("Initialize");

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
                for (auto& [group_grid_id, mesh] : result) {
                    auto node = Node("group" + std::to_string(group_grid_id.first), std::move(mesh));
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
        stopwatch.log_stage("LOD loop");
        out_model.eraseEmptyNodes();
        out_model.assignNodeHierarchy();
        stopwatch.log_stage("Node arrange");

        // テクスチャを結合します。
        if (options.enable_texture_packing) {
            TexturePacker packer(options.texture_packing_resolution, options.texture_packing_resolution);
            packer.process(out_model);
        }
        stopwatch.log_stage("Texture packing");

        // 現在の都市モデルが地形であるなら、衛星写真または地図用のUVを付与し、地図タイルをダウンロードします。
        auto package = GmlFile(city_model.getGmlPath()).getPackage();
        if(package == PredefinedCityModelPackage::Relief && options.attach_map_tile) {
            const auto gml_path = fs::u8path(city_model.getGmlPath());
            const auto map_download_dest = gml_path.parent_path() / (gml_path.filename().u8string() + "_map");
            MapAttacher().attach(out_model, options.map_tile_url, map_download_dest, options.map_tile_zoom_level,
                                 geo_reference);
        }
        stopwatch.log_stage("Map attach");
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
        extractInner(out_model, city_model, options, { plateau::geometry::Extent::all() }, nullptr);
    }

    void MeshExtractor::extract(Model& out_model, const citygml::CityModel& city_model, const MeshExtractOptions& options, const std::shared_ptr<citygml::CityGMLLogger>& logger) {
        extractInner(out_model, city_model, options, { plateau::geometry::Extent::all() }, logger);
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

        extractInner(out_model, city_model, options, extents, nullptr);
    }

    void MeshExtractor::extractInExtents(Model& out_model, const citygml::CityModel& city_model, const MeshExtractOptions& options, const std::vector<plateau::geometry::Extent>& extents, const std::shared_ptr<citygml::CityGMLLogger>& logger) {
        extractInner(out_model, city_model, options, extents, logger);
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

    /// extentsの幅と奥行き(と高さ)の長さを multiplier 倍にします。
    std::vector<plateau::geometry::Extent> MeshExtractor::extendExtents(const std::vector<plateau::geometry::Extent>& src_extents, float multiplier) {
        auto result = std::vector<plateau::geometry::Extent>();
        result.reserve(src_extents.size());
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
}
