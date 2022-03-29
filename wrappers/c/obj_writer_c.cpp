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
        try {
            return new ObjWriter;
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occurred." << std::endl;
        }
        return nullptr;
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_delete_obj_writer(ObjWriter* obj_writer) {
        try {
            delete obj_writer;
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occurred." << std::endl;
        }
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_write(ObjWriter* obj_writer, const char* obj_path, const CityModelHandle* city_model, const char* gml_path) {
        try {
            // TODO: replace '\\' -> '/' in ObjWriter
            obj_writer->write(obj_path, city_model->getCityModel(), gml_path);
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occurred." << std::endl;
        }
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_set_merge_mesh_flg(ObjWriter* obj_writer, const bool value) {
        try {
            obj_writer->setMergeMeshFlg(value);
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occured." << std::endl;
        }
    }

    LIBPLATEAU_C_EXPORT bool LIBPLATEAU_C_API plateau_obj_writer_get_merge_mesh_flg(ObjWriter* obj_writer) {
        try {
            return obj_writer->getMergeMeshFlg();
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occured." << std::endl;
        }
        return false;
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_set_dest_axes(ObjWriter* obj_writer, AxesConversion value) {
        try {
            obj_writer->setDestAxes(value);
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occured." << std::endl;
        }
    }


    LIBPLATEAU_C_EXPORT AxesConversion LIBPLATEAU_C_API plateau_obj_writer_get_dest_axes(ObjWriter* obj_writer) {
        try {
            return (obj_writer->getDestAxes());
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occured." << std::endl;
        }
        return AxesConversion::RUF;
    }


    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_set_valid_reference_point(ObjWriter* obj_writer, const CityModelHandle* city_model) {
        try {
            // TODO: replace '\\' -> '/' in ObjWriter
            obj_writer->setValidReferencePoint(city_model->getCityModel());
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occurred." << std::endl;
        }
    }

    LIBPLATEAU_C_EXPORT plateau_vector3d LIBPLATEAU_C_API plateau_obj_writer_get_reference_point(const ObjWriter* obj_writer) {
        try {
            // TODO: getReferencePoint‚Ì•Ô‚è’l‚ðTVec3dŒ^‚É•ÏX
            double ref[3];
            obj_writer->getReferencePoint(ref);
            return plateau_vector3d{ ref[0], ref[1], ref[2] };
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occurred." << std::endl;
        }
        return plateau_vector3d{ 0, 0, 0 };
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_set_reference_point(ObjWriter* obj_writer, const plateau_vector3d reference_point) {
        try {
            const double ref[3]{ reference_point.x, reference_point.y, reference_point.z };
            obj_writer->setReferencePoint(ref);
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "Unknown error occurred." << std::endl;
        }
    }
}
