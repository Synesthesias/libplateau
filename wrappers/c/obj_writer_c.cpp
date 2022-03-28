#include <exception>
#include <iostream>

#include <obj_writer.h>

#include "libplateau_c.h"


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

    // TODO: gml->citymodel処理のObjWriter側への委譲またはCityModelクラスのWrapper作成
    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_write(ObjWriter* obj_writer, const char* obj_path, const char* gml_path) {
        try {
            citygml::ParserParams params;
            params.optimize = true;
            const auto city_model = load(gml_path, params, nullptr);
            obj_writer->setValidReferencePoint(*city_model);
            // TODO: replace '\\' -> '/' in ObjWriter
            obj_writer->write(obj_path, *city_model, gml_path);
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
            // TODO: getReferencePointの返り値をTVec3d型に変更
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

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_obj_writer_set_reference_point(ObjWriter* obj_writer, plateau_vector3d reference_point) {
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
