#include "area_mesh_factory.h"

#include <plateau/polygon_mesh/primary_city_object_types.h>
#include <plateau/polygon_mesh/mesh_factory.h>
#include <plateau/polygon_mesh/polygon_mesh_utils.h>

namespace {
    using namespace plateau;
    using namespace polygonMesh;

    /**
    * グリッド番号と、そのグリッドに属する CityObject のリストを対応付ける辞書です。
    */
    using GridIDToObjectsMap = std::map<unsigned, std::list<const citygml::CityObject*>>;
    using GroupIDToObjectsMap = GridIDToObjectsMap;

    /**
     * cityModelのEnvelope(範囲)を指定のグリッド数(x,y)で分割したとき、
     * positionは何番目のグリッドに属するかを計算します。
     */
    int getGridId(const citygml::Envelope& city_envelope, const TVec3d& position,
        const int grid_num_x, const int grid_num_y) {

        const auto lower = city_envelope.getLowerBound();
        const auto upper = city_envelope.getUpperBound();
        int grid_x = static_cast<int>((position.x - lower.x) * grid_num_x / (upper.x - lower.x));
        int grid_y = static_cast<int>((position.y - lower.y) * grid_num_y / (upper.y - lower.y));
        if (grid_x < 0) grid_x = 0;
        if (grid_y < 0) grid_y = 0;
        if (grid_x >= grid_num_x) grid_x = grid_num_x - 1;
        if (grid_y >= grid_num_y) grid_y = grid_num_y - 1;
        const int grid_id = grid_x + grid_y * grid_num_x;
        return grid_id;
    }

    /**
     * key が グリッド番号であり、 value は　CityObjectのvector であるmapを初期化します。
     */
    GridIDToObjectsMap initGridIDToObjectsMap(const int grid_num_x, const int grid_num_y) {
        const int grid_num = grid_num_x * grid_num_y;
        auto grid_id_to_objects_map = GridIDToObjectsMap();
        for (int i = 0; i < grid_num; i++) {
            grid_id_to_objects_map.emplace(i, std::list<const citygml::CityObject*>());
        }
        return grid_id_to_objects_map;
    }


    /**
     * city_objects の各CityObjectが位置の上でどのグリッドに属するかを求め、gridIdToObjsMapに追加することでグリッド分けします。
     * また ImportID を割り振ります。
     * extentの範囲外のものは除外します（除外する設定の場合）。
     */
    GridIDToObjectsMap classifyCityObjectsToGrid(const citygml::ConstCityObjects& city_objects, const citygml::Envelope& city_envelope,
                                           const MeshExtractOptions& options) {
        auto grid_id_to_objects_map = initGridIDToObjectsMap(options.grid_count_of_side, options.grid_count_of_side);
        for (const auto co : city_objects) {
            // 範囲外、または位置不明ならスキップします（スキップする設定の場合）。
            if (options.exclude_city_object_outside_extent && !options.extent.contains(*co))
                continue;

            const int grid_id = getGridId(city_envelope, PolygonMeshUtils::cityObjPos(*co), options.grid_count_of_side, options.grid_count_of_side);
            grid_id_to_objects_map.at(grid_id).push_back(co);
        }
        return grid_id_to_objects_map;
    }
}

namespace plateau::polygonMesh {
    using namespace citygml;

    GridMergeResult
        AreaMeshFactory::gridMerge(const CityModel& city_model, const MeshExtractOptions& options, unsigned lod,
                              const geometry::GeoReference& geo_reference) {
        // city_model に含まれる 主要地物 をグリッドに分類します。
        const auto& all_primary_city_objects =
            city_model.getAllCityObjectsOfType(PrimaryCityObjectTypes::getPrimaryTypeMask());
        const auto& city_envelope = city_model.getEnvelope();
        auto grid_id_to_primary_objects_map = classifyCityObjectsToGrid(all_primary_city_objects, city_envelope, options);
        
        // グリッドをさらに分割してグループにします。
        // グループの分割基準:
        // 仕様上存在しうる最大LODをm として、各オブジェクトを次のグループに分けます。
        // - 同じオブジェクトが (0, 1, 2, ..., m) のLODであるポリゴンをどれも有する つまり 全LODに対応する
        // - 同じオブジェクトが (0, 1, 2, ..., m-1) のLODであるポリゴンをどれも有し、mを有しない つまり 最大LODを除く全LODに対応する
        // - ...
        // - 同じオブジェクトが (0, 1) のLODであるポリゴンをどれも有し、(2, 3, ..., m)のLODであるポリゴンをどれも有しない つまり LOD 0, 1のみに対応する
        // - 同じオブジェクトが (0) のLODであるポリゴンを有し、(1, 2, 3,  ..., m) のLODであるポリゴンをどれも有しない つまり LOD 0 のみに対応する
        // 今の仕様上、mは3なので、各グリッドは最大4つに分割されます。
        // したがって、grid 0 と group 0 to 3 が対応します。 grid1 と group 4 to 7 が対応します。
        // 一般化すると、 grid i と group (i*m) to (i*(m+1)-1)が対応します。
        // 仕様上、あるオブジェクトのLOD i が存在すれば、同じオブジェクトの lod 0 to i-1 がすべて存在します。したがって、各オブジェクトは必ず上記グループのどれか1つに該当するはずです。
        // そのようにグループ分けする利点は、
        // 「高いLODを表示したが、低いLODにしか対応していない箇所が穴になってしまう」という状況で、穴をちょうど埋める範囲の低LODグループが存在することです。
        auto group_id_to_primary_objects_map = GroupIDToObjectsMap();
        for (const auto& [grid_id, primary_objects_in_grid] : grid_id_to_primary_objects_map) {
            for (const auto& primary_object : primary_objects_in_grid) {
                // この CityObject について、最大でどのLODまで存在するか確認します。
                unsigned max_lod_in_obj = PolygonMeshUtils::max_lod_in_specification_;
                for (unsigned target_lod = lod + 1; target_lod <= PolygonMeshUtils::max_lod_in_specification_; ++target_lod) {
                    bool target_lod_exists =
                        PolygonMeshUtils::findFirstPolygon(primary_object, target_lod) != nullptr;
                    if (!target_lod_exists) {
                        max_lod_in_obj = target_lod - 1;
                        break;
                    }
                }
                // グループに追加します。
                unsigned group_id = grid_id * (PolygonMeshUtils::max_lod_in_specification_ + 1) + max_lod_in_obj;
                if (group_id_to_primary_objects_map.find(group_id) == group_id_to_primary_objects_map.end())
                    group_id_to_primary_objects_map[group_id] = std::list<const CityObject*>();

                group_id_to_primary_objects_map.at(group_id).push_back(primary_object);
            }
        }

        // グループごとにメッシュを結合します。
        auto merged_meshes = GridMergeResult();
        // グループごとのループ
        for (const auto& [group_id, primary_objects] : group_id_to_primary_objects_map) {
            // 1グループのメッシュ生成
            MeshFactory mesh_factory(nullptr, options, geo_reference);

            // グループ内の各主要地物のループ
            for (const auto& primary_object : primary_objects) {
                if (MeshExtractor::shouldContainPrimaryMesh(lod, *primary_object)) {
                    mesh_factory.addPolygonsInPrimaryCityObject(*primary_object, lod, city_model.getGmlPath());
                }

                if (lod >= 2) {
                    // 主要地物の子である各最小地物をメッシュに加えます。
                    auto atomic_objects = PolygonMeshUtils::getChildCityObjectsRecursive(*primary_object);
                    mesh_factory.addPolygonsInAtomicCityObjects(*primary_object, atomic_objects, lod, city_model.getGmlPath());
                }
                mesh_factory.incrementPrimaryIndex();
            }
            merged_meshes.emplace(group_id, mesh_factory.releaseMesh());
        }
        return merged_meshes;
    }
}
