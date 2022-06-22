//#include <exception>
//#include <iostream>
//
//#include "libplateau_c.h"
//#include "city_model_c.h"
//
//using namespace libplateau;
//
//extern "C" {
//    struct plateau_vector3d {
//        double x;
//        double y;
//        double z;
//    };
//
//    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_obj_writer(ObjWriter** out_obj_writer) {
//        API_TRY{
//            *out_obj_writer = new ObjWriter;
//            return APIResult::Success;
//        }
//        API_CATCH;
//        return APIResult::ErrorUnknown;
//    }
//
//
//    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_delete_obj_writer(ObjWriter* obj_writer) {
//        API_TRY{
//            delete obj_writer;
//        }
//        API_CATCH;
//    }
//
//    //LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_obj_writer_write(ObjWriter* obj_writer, const char* obj_path, const CityModelHandle* city_model, const char* gml_path) {
//    //    API_TRY{
//    //        obj_writer->convert(obj_path, gml_path, city_model->getCityModel());
//    //        return APIResult::Success;
//    //    }
//    //    API_CATCH;
//    //    return APIResult::ErrorUnknown;
//    //}
//
//    DLL_PTR_FUNC(plateau_obj_writer_get_dll_logger,
//                 ObjWriter,
//                 PlateauDllLogger,
//                 handle->getLogger())
//}
