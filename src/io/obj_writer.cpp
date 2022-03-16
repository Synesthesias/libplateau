#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <direct.h>
#include <sys/stat.h>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/geometry.h>
#include <citygml/polygon.h>
#include <citygml/texture.h>

#include "obj_writer.h"
#include "polar_to_plane_cartesian.h"

void ObjWriter::write(const std::string& obj_file_path, const citygml::CityModel& city_model, const std::string& gml_file_path) {

    gml_file_path_ = gml_file_path;
    obj_file_path_ = obj_file_path;
    unsigned int v_offset = 0, t_offset = 0;

    ofs_ = std::ofstream(obj_file_path_);
    if (!ofs_.is_open()) {
        throw std::runtime_error(std::string("Failed to open : ") + obj_file_path_);
    }

    const size_t dir_i = obj_file_path_.find_last_of("/");
    const size_t file_i = obj_file_path_.find_last_of(".");
    const std::string mat_file_path = obj_file_path_.substr(0, file_i) + ".mtl";
    std::string mat_file_name;
    if (dir_i == std::string::npos) {
        mat_file_name = obj_file_path_.substr(0, file_i) + ".mtl";
    } else {
        mat_file_name = obj_file_path_.substr(dir_i + 1, file_i - dir_i - 1) + ".mtl";
    }

    ofs_mat_ = std::ofstream(mat_file_path);
    if (!ofs_mat_.is_open()) {
        throw std::runtime_error(std::string("Failed to open : ") + mat_file_path);
    }

    ofs_ << std::fixed << std::setprecision(6);
    const auto rc = city_model.getNumRootCityObjects();
    std::cout << "NumRootCityObjects : " << rc << std::endl;
    ofs_ << "mtllib " << mat_file_name << std::endl;
    for (const auto& root_object : city_model.getRootCityObjects()) {
        std::cout << "建物ID : " << root_object->getAttribute("建物ID") << std::endl;
        std::cout << "RootID : " << root_object->getId() << std::endl;
        if (merge_mesh_flg_) {
            ofs_ << "g " << root_object->getId() << std::endl;
        }
        const auto cc = root_object->getChildCityObjectsCount();
        std::cout << "ChildCityObjectsCount : " << cc << std::endl;
        for (unsigned int i = 0; i < cc; i++) {
            const auto& target_object = root_object->getChildCityObject(i);
            if (!merge_mesh_flg_) {
                ofs_ << "g " << target_object.getId() << std::endl;
            }
            std::cout << "ChildID : " << target_object.getId() << std::endl;
            const auto gc = target_object.getGeometriesCount();
            for (unsigned int j = 0; j < gc; j++) {
                const auto pc = target_object.getGeometry(j).getPolygonsCount();
                for (unsigned int k = 0; k < pc; k++) {
                    const auto v_cnt = writeVertices(target_object.getGeometry(j).getPolygon(k)->getVertices());

                    const auto citygmlTex = target_object.getGeometry(j).getPolygon(k)->getTextureFor("rgbTexture");
                    bool tex_flg = false;
                    unsigned int t_cnt = 0;
                    if (citygmlTex) {
                        tex_flg = true;
                        t_cnt = writeUVs(target_object.getGeometry(j).getPolygon(k)->getTexCoordsForTheme("rgbTexture", true));
                        writeMaterial(citygmlTex->getUrl());
                    }

                    writeIndices(target_object.getGeometry(j).getPolygon(k)->getIndices(), v_offset, t_offset, tex_flg);
                    v_offset += v_cnt;
                    t_offset += t_cnt;
                }
            }
        }
    }
    ofs_.close();
}

unsigned int ObjWriter::writeVertices(const std::vector<TVec3d>& vertices) {
    int cnt = 0;
    std::for_each(vertices.cbegin(), vertices.cend(), [&](const TVec3d& v) {
        double xyz[3];
        for (int i = 0; i < 3; i++) xyz[i] = v[i];
        polar_to_plane_cartesian().convert(xyz);
        for (int i = 0; i < 3; i++) xyz[i] -= ref_point_[i];
        if (axes_ == AxesConversion::WNU) {
            ofs_ << "v " << xyz[0] << " " << xyz[1] << " " << xyz[2] << std::endl;
        }
        else if (axes_ == AxesConversion::RUF) {
            ofs_ << "v " << -xyz[0] << " " << xyz[2] << " " << xyz[1] << std::endl;
        } 
        else {
            throw std::runtime_error("Unknown axes type.");
        }
        cnt++;
    });
    return cnt;
}

unsigned int ObjWriter::writeUVs(const std::vector<TVec2f>& uvs) {
    int cnt = 0;
    std::for_each(uvs.cbegin(), uvs.cend(), [&](const TVec2f& uv) {
        ofs_ << "vt " << uv.x << " " << uv.y << std::endl;
        cnt++;
    });
    return cnt;
}

void ObjWriter::writeIndices(const std::vector<unsigned int>& indices, unsigned int ix_offset, unsigned int tx_offset, bool tex_flg) {
    int i = 0;
    for (auto itr = indices.begin(); itr != indices.end(); itr++) {
        if (i == 0) ofs_ << "f ";
        if (tex_flg) {
            ofs_ << *itr + 1 + ix_offset << "/" << *itr + 1 + tx_offset << " ";
        } else {
            ofs_ << *itr + 1 + ix_offset << " ";
        }
        i++;
        if (i == 3) {
            i = 0;
            ofs_ << std::endl;
        }
    }
}

void ObjWriter::writeMaterial(const std::string& tex_path) {
    size_t path_i = tex_path.find_last_of("/") + 1;
    size_t ext_i = tex_path.find_last_of(".");
    std::string mat_name = tex_path.substr(path_i, ext_i - path_i);

    bool newmat_flg = true;
    for (const auto& item : mat_list_) {
        if (item == mat_name) {
            newmat_flg = false;
            break;
        }
    }

    if (newmat_flg || !merge_mesh_flg_) {
        ofs_ << "usemtl " << mat_name << std::endl;
    }

    if (newmat_flg) {
        ofs_mat_ << "newmtl " << mat_name << std::endl;
        ofs_mat_ << "map_Kd ./" << tex_path << std::endl << std::endl;
        mat_list_.push_back(mat_name);

        std::string path_from = gml_file_path_.substr(0, gml_file_path_.find_last_of("/") + 1) + tex_path;
        std::string path_to;

        size_t dir_i = obj_file_path_.find_last_of("/");
        if (dir_i == std::string::npos) {
            path_to = tex_path;
        } else {
            path_to = obj_file_path_.substr(0, dir_i + 1) + tex_path;
        }

        struct stat statBuf;
        std::string to_dir = path_to.substr(0, path_to.find_last_of("/"));
        if (stat(to_dir.c_str(), &statBuf) != 0) {
            if (_mkdir(to_dir.c_str()) != 0) {
                throw std::runtime_error(std::string("Failed to make directory : ") + to_dir);
            }
        }
        std::ifstream ifstr(path_from, std::ios::binary);
        if (!ifstr.is_open()) {
            throw std::runtime_error(std::string("Failed to open : ") + path_from);
        }
        std::ofstream ofstr(path_to, std::ios::binary);
        if (!ofstr.is_open()) {
            throw std::runtime_error(std::string("Failed to open : ") + path_to);
        }
        ofstr << ifstr.rdbuf();
    }
}

void ObjWriter::setMergeMeshFlg(bool value) {
    merge_mesh_flg_ = value;
}

void ObjWriter::setDestAxes(AxesConversion value) {
    axes_ = value;
}

void ObjWriter::setValidReferencePoint(const citygml::CityModel& city_model) {
    auto lower_bound = city_model.getEnvelope().getLowerBound();
    auto upper_bound = city_model.getEnvelope().getUpperBound();

    polar_to_plane_cartesian().convert(lower_bound);
    polar_to_plane_cartesian().convert(upper_bound);
    
    ref_point_[0] = (lower_bound.x + upper_bound.x)/2.0;
    ref_point_[1] = (lower_bound.y + upper_bound.y) / 2.0;
    ref_point_[2] = lower_bound.z;

    std::cout << "Set ReferencePoint @ " << ref_point_[0] << ", " << ref_point_[1] << ", " << ref_point_[2] << std::endl;
}

void ObjWriter::getReferencePoint(double xyz[]) const{
    for (int i = 0; i < 3; i++) xyz[i] = ref_point_[i];
}

void ObjWriter::setReferencePoint(const double xyz[]) {
    for (int i = 0; i < 3; i++) ref_point_[i] = xyz[i];
    std::cout << "Set ReferencePoint @ " << ref_point_[0] << ", " << ref_point_[1] << ", " << ref_point_[2] << std::endl;
}