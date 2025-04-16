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

        if (!options.is_polar_coordinate_system) {
            // 平面直角座標系の判定

            //plateau::geometry::GeoCoordinate refPoint = plateau::geometry::ReferencePointFactory::GetReferencePoint(options.epsg_code);
            //plateau::geometry::GeoReference geo_ref1(options.coordinate_zone_id);
            //const auto epsg_offset = geo_ref1.getOffset(options.epsg_code);
            //geo_ref1.setReferencePoint(epsg_offset);

            plateau::geometry::GeoReference geo_ref(options.coordinate_zone_id, options.reference_point, options.unit_scale, options.mesh_axes); 
            const auto epsg_offset = geo_ref.getOffset(options.epsg_code);

            //const auto diff = options.reference_point - epsg_offset;
            //plateau::geometry::GeoReference geo_ref3(options.coordinate_zone_id, options.reference_point - diff , options.unit_scale, options.mesh_axes);

            const auto meshcode_offset = GeoReference::convertAxisToENU(options.mesh_axes, options.reference_point);
            //const auto original_ref = geo_ref2.unproject(options.reference_point);

            plateau::geometry::GeoReference geo_ref3(options.coordinate_zone_id, TVec3d(), options.unit_scale, options.mesh_axes);
            const auto original_ref = geo_ref3.unproject(options.reference_point);


            //int zone_id = options.coordinate_zone_id;
            int zone_id = 8; //8固定
            plateau::geometry::GeoReference geo(zone_id);

            GeoCoordinate ref_point_epsg = ReferencePointFactory::GetReferencePoint(options.epsg_code); //EPSGの基準点
            const auto prj_epsg = geo.project(ref_point_epsg);
            plateau::geometry::GeoReference geo_ref_epsg(zone_id, prj_epsg, options.unit_scale);

            GeoCoordinate ref_point_zukaku(37.4258, 138.7378, 0); //08EE751の中心
            const auto prj2 = geo.project(ref_point_zukaku);
            plateau::geometry::GeoReference geo_ref_zukaku(zone_id, prj2, options.unit_scale);

            //GeoCoordinate ref_point_epsg_static(36, 138.5, 0); //EPSG:10169 の基準点
            //const auto prj3 = geo.project(ref_point_epsg_static);
            //plateau::geometry::GeoReference geo_ref_epsg_static(zone_id, prj3, options.unit_scale);

            try {
                const auto pos = PolygonMeshUtils::cityObjPos(city_obj);   

                //const auto unprojected = geo_ref.unproject(pos + epsg_offset);

                //const auto unprojected_epsg = geo_ref_epsg.unproject(pos);
                //const auto unprojected_zukaku = geo_ref_zukaku.unproject(pos);

                //const auto unprojected_epsg_offset = geo_ref_epsg.unproject(pos + epsg_offset);
                //const auto unprojected_zukaku_offset = geo_ref_zukaku.unproject(pos + epsg_offset);

                // converted
                //const auto pos_converted = GeoReference::convertAxisToENU(geometry::CoordinateSystem::WUN, pos);
                const TVec3d pos_converted = { pos.y, pos.x, pos.z }; //x,y反転

                const auto unprojected1 = geo_ref.unproject(pos_converted);

                const auto unprojected_epsg2 = geo_ref_epsg.unproject(pos_converted);
                const auto unprojected_zukaku2 = geo_ref_zukaku.unproject(pos_converted);

                for (const auto& extent : extents) {
                    if (extent.contains(unprojected_epsg2))
                        return false;
                }

                //auto pos = PolygonMeshUtils::cityObjPos(city_obj);
                //for (const auto& extent : extents) {
                //    if (extent.contains(geo_ref.unproject(pos)))
                //        return false;
                //}
            }
            catch (std::invalid_argument& e) {}

            //Temp Debug =======================
            //return false;
            //Temp Debug =======================
        }
        else {
            for (const auto& extent : extents) {
                if (extent.contains(city_obj))
                    return false;
            }
        }
        return true;
    }

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
        Model& out_model, const CityModel& city_model,
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
