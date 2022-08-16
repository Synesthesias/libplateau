#include "libplateau_c.h"
#include "city_model_c.h"
#include <plateau/io/mesh_merger.h>
using namespace libplateau;

extern "C"{

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_merger_new(
            MeshMerger** out_mesh_merger_ptr
            ){
        *out_mesh_merger_ptr = new MeshMerger();
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_merger_delete(
            MeshMerger* mesh_merger_handle
            ){
        delete mesh_merger_handle;
        return APIResult::Success;
    }


    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_merger_grid_merge(
            MeshMerger* mesh_merger_handle,
            const CityModelHandle* city_model_handle,
            CityObject::CityObjectsType target_type_mask,
            int grid_num_x,
            int grid_num_y,
            int* out_num_polygons,
            PlateauDllLogger* logger){
        API_TRY{
            auto sharedLogger = std::make_shared<PlateauDllLogger>(*logger);
            mesh_merger_handle->gridMerge(city_model_handle->getCityModelPtr().get(), target_type_mask, grid_num_x, grid_num_y,sharedLogger);
            auto gridMergeResult = mesh_merger_handle->getLastGridMergeResult();
            *out_num_polygons = (int)gridMergeResult->size();
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    /**
     * MeshMerger::gridMerge の結果を Polygonのアドレスの配列で返します。
     */
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_mesh_merger_get_last_result_of_grid_merge(
            MeshMerger* mesh_merger_handle,
            PlateauPolygon** out_plateau_polygons
    ){
        API_TRY{
            auto last_merge_result = mesh_merger_handle->getLastGridMergeResult();
            int num_polygons = (int)last_merge_result->size();
            for(int i=0; i<num_polygons; i++){
                out_plateau_polygons[i] = last_merge_result->at(i);
            }
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }
}