#include <exception>
#include <iostream>

#include <obj_writer.h>

#include "libplateau_c.h"
#include "city_model_c.h"

extern "C" {
    struct plateau_vector3d {
        double x;
        double y;
        double z;
    };

    LIBPLATEAU_C_EXPORT ObjWriter* LIBPLATEAU_C_API plateau_create_obj_writer() {
        API_TRY{
            return new ObjWriter;
        }
        API_CATCH;
        return nullptr;
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_delete_obj_writer(ObjWriter* obj_writer) {
        API_TRY{
            delete obj_writer;
        }
        API_CATCH;
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_write(ObjWriter* obj_writer, const char* obj_path, const CityModelHandle* city_model, const char* gml_path) {
        API_TRY{
            // TODO: replace '\\' -> '/' in ObjWriter
            obj_writer->write(obj_path, city_model->getCityModel(), gml_path);
        }
        API_CATCH;
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_set_merge_mesh_flg(ObjWriter* obj_writer, const bool value) {
        API_TRY{
            obj_writer->setMergeMeshFlg(value);
        }
        API_CATCH;
    }

    LIBPLATEAU_C_EXPORT bool LIBPLATEAU_C_API plateau_obj_writer_get_merge_mesh_flg(ObjWriter* obj_writer) {
        API_TRY{
            return obj_writer->getMergeMeshFlg();
        }
        API_CATCH;
        return false;
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_set_dest_axes(ObjWriter* obj_writer, AxesConversion value) {
        API_TRY{
            obj_writer->setDestAxes(value);
        }
        API_CATCH;
    }


    LIBPLATEAU_C_EXPORT AxesConversion LIBPLATEAU_C_API plateau_obj_writer_get_dest_axes(ObjWriter* obj_writer) {
        API_TRY{
            return (obj_writer->getDestAxes());
        }
        API_CATCH;
        return AxesConversion::RUF;
    }


    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_set_valid_reference_point(ObjWriter* obj_writer, const CityModelHandle* city_model) {
        API_TRY{
            // TODO: replace '\\' -> '/' in ObjWriter
            obj_writer->setValidReferencePoint(city_model->getCityModel());
        }
        API_CATCH;
    }

    LIBPLATEAU_C_EXPORT plateau_vector3d LIBPLATEAU_C_API plateau_obj_writer_get_reference_point(const ObjWriter* obj_writer) {
        API_TRY{
            // TODO: getReferencePoint‚Ì•Ô‚è’l‚ðTVec3dŒ^‚É•ÏX
            double ref[3];
            obj_writer->getReferencePoint(ref);
            return plateau_vector3d{ ref[0], ref[1], ref[2] };
        }
        API_CATCH;
        return plateau_vector3d{ 0, 0, 0 };
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_set_reference_point(ObjWriter* obj_writer, const plateau_vector3d reference_point) {
        API_TRY{
            const double ref[3]{ reference_point.x, reference_point.y, reference_point.z };
            obj_writer->setReferencePoint(ref);
        }
        API_CATCH;
    }
}
