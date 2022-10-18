#include "grid_merger.h"
#include <plateau/polygon_mesh/primary_city_object_types.h>
#include "plateau/mesh_writer/obj_writer.h"
#include "mesh_merger.h"
#include <plateau/polygon_mesh/polygon_mesh_utils.h>

namespace plateau::polygonMesh {
    using namespace citygml;
/**
 * インポートした各CityObject についてインポート番号を記憶するためのクラスです。
 * ID情報をUV2に埋め込む時に利用します。
 * TODO UV2からデータを取り出す機能は未実装です。
 */
    class CityObjectWithImportID {
    public:
        CityObjectWithImportID(const CityObject* city_object, int primary_import_id, int secondary_import_id);

        const CityObject* getCityObject() const { return city_object_; }

        int getPrimaryImportID() const { return primary_import_id_; }

        int getSecondaryImportID() const { return secondary_import_id_; }

    private:
        const CityObject* const city_object_;
        int primary_import_id_;
        int secondary_import_id_;
    };

    CityObjectWithImportID::CityObjectWithImportID(const CityObject* const city_object, int primary_import_id,
                                                   int secondary_import_id) :
            city_object_(city_object),
            primary_import_id_(primary_import_id),
            secondary_import_id_(secondary_import_id) {
    }

/**
* グリッド番号と、そのグリッドに属する CityObject のリストを対応付ける辞書です。
*/
    using GridIDToObjsMap = std::map<unsigned, std::list<CityObjectWithImportID>>;
    using GroupIDToObjsMap = GridIDToObjsMap;


    namespace {

        /**
         * cityModelのEnvelope(範囲)を指定のグリッド数(x,y)で分割したとき、
         * positionは何番目のグリッドに属するかを計算します。
         */
        int
        getGridId(const Envelope& city_envelope, const TVec3d position, const int grid_num_x, const int grid_num_y) {
            auto lower = city_envelope.getLowerBound();
            auto upper = city_envelope.getUpperBound();
            int grid_x = (int) ((position.x - lower.x) * grid_num_x / (upper.x - lower.x));
            int grid_y = (int) ((position.y - lower.y) * grid_num_y / (upper.y - lower.y));
            if (grid_x < 0) grid_x = 0;
            if (grid_y < 0) grid_y = 0;
            if (grid_x >= grid_num_x) grid_x = grid_num_x - 1;
            if (grid_y >= grid_num_y) grid_y = grid_num_y - 1;
            int grid_id = grid_x + grid_y * grid_num_x;
            return grid_id;
        }

        /**
         * key が グリッド番号であり、 value は　CityObjectのvector であるmapを初期化します。
         */
        GridIDToObjsMap initGridIdToObjsMap(const int grid_num_x, const int grid_num_y) {
            int grid_num = grid_num_x * grid_num_y;
            auto grid_id_to_objs_map = GridIDToObjsMap();
            for (int i = 0; i < grid_num; i++) {
                grid_id_to_objs_map.emplace(i, std::list<CityObjectWithImportID>());
            }
            return grid_id_to_objs_map;
        }


        /**
         * city_objs の各CityObjectが位置の上でどのグリッドに属するかを求め、gridIdToObjsMapに追加することでグリッド分けします。
         * また ImportID を割り振ります。
         * extentの範囲外のものは除外します（除外する設定の場合）。
         */
        GridIDToObjsMap classifyCityObjsToGrid(const ConstCityObjects& city_objs, const Envelope& city_envelope,
                                               const MeshExtractOptions& options) {
            auto grid_id_to_objs_map = initGridIdToObjsMap(options.grid_count_of_side, options.grid_count_of_side);
            int primary_import_id = 0;
            for (auto co: city_objs) {
                // 範囲外、または位置不明ならスキップします（スキップする設定の場合）。
                if(options.exclude_city_object_outside_extent && !options.extent.contains(*co)) continue;

                int grid_id = getGridId(city_envelope, PolygonMeshUtils::cityObjPos(*co), options.grid_count_of_side, options.grid_count_of_side);
                auto city_obj_with_import_id = CityObjectWithImportID(co, primary_import_id, -1);
                grid_id_to_objs_map.at(grid_id).push_back(city_obj_with_import_id);
                primary_import_id++;
            }
            return grid_id_to_objs_map;
        }

    }

    GridMergeResult
    GridMerger::gridMerge(const CityModel& city_model, const MeshExtractOptions& options, unsigned lod,
                          const GeoReference& geo_reference) {
        // city_model に含まれる 主要地物 をグリッドに分類します。
        const auto& primary_city_objs = city_model.getAllCityObjectsOfType(
                PrimaryCityObjectTypes::getPrimaryTypeMask());
        const auto& city_envelope = city_model.getEnvelope();
        auto grid_id_to_objs_map = classifyCityObjsToGrid(primary_city_objs, city_envelope, options);

        // 主要地物の子（最小地物）を親と同じグリッドに追加します。
        // 意図はグリッドの端で同じ建物が分断されないようにするためです。
        for (const auto& pair: grid_id_to_objs_map) {
            unsigned grid_id = pair.first;
            const auto& primary_objs = pair.second;
            for (const auto& primary_obj: primary_objs) {
                int primary_id = primary_obj.getPrimaryImportID();
                auto atomic_objs = PolygonMeshUtils::getChildCityObjectsRecursive(*primary_obj.getCityObject());
                int secondary_id = 0;
                for (auto atomic_obj: atomic_objs) {
                    auto atomic_obj_with_id = CityObjectWithImportID{atomic_obj, primary_id, secondary_id};
                    grid_id_to_objs_map.at(grid_id).push_back(atomic_obj_with_id);
                    secondary_id++;
                }
            }
        }

        // グリッドをさらに分割してグループにします。
        // グループの分割基準:
        // 今のLODをn、仕様上存在しうる最大LODをm として、各オブジェクトを次のグループに分けます。
        // - 同じオブジェクトが (0, 1, 2, ..., m) のLODであるポリゴンをどれも有する
        // - 同じオブジェクトが (0, 1, 2, ..., m-1) のLODであるポリゴンをどれも有し、mを有しない
        // - ...
        // - 同じオブジェクトが (0, 1) のLODであるポリゴンをどれも有し、(2, 3, ..., m)のLODであるポリゴンをどれも有しない
        // - 同じオブジェクトが (0) のLODであるポリゴンを有し、(1, 2, 3,  ..., m) のLODであるポリゴンをどれも有しない
        // 今の仕様上、mは3なので、各グリッドは最大4つに分割されます。
        // したがって、grid 0 と group 0 to 3 が対応します。 grid1 と group 4 to 7 が対応します。
        // 一般化すると、 grid i と group (i*m) to (i*(m+1)-1)が対応します。
        // 仕様上、あるオブジェクトのLOD i が存在すれば、同じオブジェクトの lod 0 to i-1 がすべて存在します。したがって、各オブジェクトは必ず上記グループのどれか1つに該当するはずです。
        // そのようにグループ分けする利点は、
        // 「高いLODを表示したいけど、低いLODにしか対応していない箇所が穴になってしまう」という状況で、穴をちょうど埋める範囲の低LODグループが存在することです。
        auto group_id_to_objs_map = GroupIDToObjsMap();
        for (const auto& grid_objs_pair: grid_id_to_objs_map) {
            auto grid_id = grid_objs_pair.first;
            const auto& grid_objs = grid_objs_pair.second;
            for (const auto& obj: grid_objs) {
                // この CityObj について、最大でどのLODまで存在するか確認します。
                unsigned max_lod_in_obj = PolygonMeshUtils::max_lod_in_specification_;
                for (unsigned check_lod = lod + 1; check_lod <= PolygonMeshUtils::max_lod_in_specification_; ++check_lod) {
                    bool polygon_in_lod_exists =
                            (check_lod == lod) || (PolygonMeshUtils::findFirstPolygon(obj.getCityObject(), check_lod) != nullptr);
                    if (!polygon_in_lod_exists) {
                        max_lod_in_obj = check_lod - 1;
                        break;
                    }
                }
                // グループに追加します。
                unsigned group_id = grid_id * (PolygonMeshUtils::max_lod_in_specification_ + 1) + max_lod_in_obj;
                if (group_id_to_objs_map.find(group_id) == group_id_to_objs_map.end()) {
                    group_id_to_objs_map[group_id] = std::list<CityObjectWithImportID>();
                }
                group_id_to_objs_map.at(group_id).push_back(obj);
            }
        }

        // グループごとにメッシュを結合します。
        auto merged_meshes = GridMergeResult();
        // グループごとのループ
        for (const auto& group: group_id_to_objs_map) {
            auto group_id = group.first;
            const auto& group_objs = group.second;
            // グループ内でマージする Mesh の新規作成
            auto group_mesh = Mesh("group" + std::to_string(group_id));
            // グループ内の各オブジェクトのループ
            for (const auto& city_obj: group_objs) {
                auto polygons = MeshMerger::findAllPolygons(*city_obj.getCityObject(), lod);
                // オブジェクト内の各ポリゴンのループ
                for (const auto& poly: polygons) {
                    // 各ポリゴンを結合していきます。
                    // importID をメッシュに残すためにuv2, uv_3 を利用しています。UVなので2次元floatの値を取りますが、実際に伝えたい値はUVごとに1つのintです。
                    const auto uv_2 = TVec2f((float) (city_obj.getPrimaryImportID()) + (float) 0.25,
                                             0); // +0.25 する理由は、floatの誤差があっても四捨五入しても切り捨てても望みのint値を得られるためです。
                    const auto uv_3 = TVec2f((float) (city_obj.getSecondaryImportID()) + (float) 0.25, 0);
                    MeshMerger::merge(group_mesh, *poly, options, geo_reference, uv_2, uv_3, city_model.getGmlPath());
                }
            }
            merged_meshes.emplace(group_id, std::move(group_mesh));
        }
        return merged_meshes;
    }
}
