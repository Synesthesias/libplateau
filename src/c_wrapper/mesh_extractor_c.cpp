#include "libplateau_c.h"
#include "city_model_c.h"
#include <plateau/geometry/mesh_extractor.h>
using namespace libplateau;
using namespace plateau::geometry;
extern "C"{

//    std::shared_ptr<Model> plateau_mesh_extractor_last_extract_result;

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_new(
            MeshExtractor** out_mesh_extractor_ptr
            ){
        *out_mesh_extractor_ptr = new MeshExtractor();
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_delete(
            MeshExtractor* mesh_extractor
            ){
        delete mesh_extractor;
        return APIResult::Success;
    }


    /**
     * MeshExtractor::extract して結果を out_model_ptr に格納します。
     * out_model_ptr は new した生ポインタになるので、deleteはDLL利用者の責任です。
     * ただしModelをdeleteすると配下のNode,Meshにもアクセスできなくなるので注意してください。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_extract(
            const MeshExtractor* const mesh_extractor,
            const CityModelHandle* const city_model_handle,
            MeshExtractOptions options,
            Model** const out_model_ptr){
        API_TRY{
            auto modelPtr =
                    mesh_extractor->extract_to_row_pointer(*city_model_handle->getCityModelPtr(), options);
            *out_model_ptr = modelPtr;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    // TODO これは仕様変更につき不要になる見込み。extractで同じ機能をまかなえるようにする。
//    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_grid_merge(
//            MeshExtractor* mesh_merger_handle,
//            const CityModelHandle* city_model_handle,
//            CityObject::CityObjectsType target_type_mask,
//            int grid_num_x,
//            int grid_num_y,
//            int* out_num_polygons,
//            PlateauDllLogger* logger){
//        API_TRY{
//            auto sharedLogger = std::make_shared<PlateauDllLogger>(*logger);
//
//            //TODO 仮
//            auto options = MeshExtractOptions(TVec3d(0,0,0), AxesConversion::WUN, MeshGranularity::PerCityModelArea, 2, 2, true, 5);
//
//            mesh_merger_handle->gridMerge(*city_model_handle->getCityModelPtr(), options, sharedLogger);
//            auto& gridMergeResult = mesh_merger_handle->getLastGridMergeResult();
//            *out_num_polygons = (int)gridMergeResult.size();
//            return APIResult::Success;
//        }
//        API_CATCH;
//        return APIResult::ErrorUnknown;
//    }

    // TODO これは仕様変更につき不要になる見込み。extractで同じ機能をまかなえるようにする。
    /**
     * MeshExtractor::gridMerge の結果を Polygonのアドレスの配列で返します。
     */
//    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_extractor_get_last_result_of_grid_merge(
//            MeshExtractor* mesh_merger_handle,
//            Mesh** out_plateau_polygons
//    ){
//        API_TRY{
//            auto& last_merge_result = mesh_merger_handle->getLastGridMergeResult();
//            int num_polygons = (int)last_merge_result.size();
//            for(int i=0; i<num_polygons; i++){
//                out_plateau_polygons[i] = &last_merge_result.at(i);
//            }
//            return APIResult::Success;
//        }
//        API_CATCH;
//        return APIResult::ErrorUnknown;
//    }
}