#include <plateau/polygon_mesh/mesh.h>
#include "libplateau_c.h"
#include <cassert>

using namespace citygml;
using namespace libplateau;
using namespace plateau::polygonMesh;

extern "C" {
    struct plateau_city_object_index {
        int primary_index;
        int atomic_index;

        static plateau_city_object_index convert_from(const CityObjectIndex& value) {
            return { value.primary_index, value.atomic_index };
        }
    };


    DLL_CREATE_FUNC(plateau_create_city_object_list,
                    CityObjectList)

    DLL_DELETE_FUNC(plateau_delete_city_object_list,
                    CityObjectList)

    DLL_STRING_PTR_FUNC(plateau_city_object_list_get_primary_id,
                        CityObjectList,
                        handle->getPrimaryGmlID(index),
                        , int index)
    
    DLL_STRING_PTR_FUNC(plateau_city_object_list_get_atomic_id,
                        CityObjectList,
                        handle->getAtomicGmlID({ index.primary_index, index.atomic_index }),
                        , plateau_city_object_index index)

    DLL_VALUE_FUNC(plateau_city_object_list_get_city_object_index,
                   CityObjectList,
                   plateau_city_object_index,
                   plateau_city_object_index::convert_from(handle->getCityObjectIndex(gml_id));
                   return APIResult::Success,
                   ,const char* const gml_id)

    DLL_VALUE_FUNC(plateau_city_object_list_get_size,
                   CityObjectList,
                   int,
                   handle->size())



/// 関数 plateau_city_object_list_get_atomic_id と似ていますが、
/// こちらはCityObjectIndexに対応する値がない場合にAPIResult::ErrorValueNotFoundを返します。
LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_city_object_list_try_get_gml_id(
                    const CityObjectList* const city_obj_list,
                    const CityObjectIndex city_obj_index,
                    const char** const out_gml_id_ptr,
                    dll_str_size_t* out_str_length){
        API_TRY{
            bool found = city_obj_list->containsCityObjectIndex(city_obj_index);
            if(!found) return APIResult::ErrorValueNotFound;
            auto& gml_id = city_obj_list->getAtomicGmlID(city_obj_index);
            *out_gml_id_ptr = gml_id.c_str();
            *out_str_length = (dll_str_size_t)gml_id.length() + 1;
            return APIResult::Success;
        }API_CATCH
        return APIResult::ErrorUnknown;
    }




    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_city_object_list_get_all_keys(
            const CityObjectList* list,
            std::vector<CityObjectIndex>* out_vector
    ) {
       API_TRY{
           list->getAllKeys(*out_vector);
           return APIResult::Success;
       } API_CATCH
           return APIResult::ErrorUnknown;
   }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_city_object_list_add(
            CityObjectList* const city_obj_list,
            const CityObjectIndex city_obj_index,
            const char* const gml_id
    ) {
        API_TRY {
            city_obj_list->add(city_obj_index, std::string(gml_id));
            return APIResult::Success;
        }
        API_CATCH
        return APIResult::ErrorUnknown;
    }
}
