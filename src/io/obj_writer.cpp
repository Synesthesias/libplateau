#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iomanip>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/geometry.h>
#include <citygml/polygon.h>
#include <citygml/texture.h>

#include "obj_writer.h"
#include "polar_to_plane_cartesian.h"

void ObjWriter::write(const std::string& file_path, const citygml::CityModel& city_model) {

    unsigned int v_cnt = 0, t_cnt = 0, cntv, cntt;
    std::vector<std::string> mat_list;
    std::ofstream ofs_mat("materials.mtl");
    if (!ofs_mat.is_open()) {
        throw std::runtime_error("Failed to open materials.mtl");
    }

    ofs_ = std::ofstream(file_path);
    if (!ofs_.is_open()) {
        throw std::runtime_error(std::string("Failed to open : ") + file_path);
    }

    ofs_ << std::fixed << std::setprecision(6);
    const auto rc = city_model.getNumRootCityObjects();
    std::cout << "NumRootCityObjects : " << rc << std::endl;
    ofs_ << "mtllib materials.mtl" << std::endl;
    for (unsigned int h = 0; h < rc; h++) {
        std::cout << "建物ID : " << city_model.getRootCityObject(h).getAttribute("建物ID") << std::endl;
        const auto cc = city_model.getRootCityObject(h).getChildCityObjectsCount();
        std::cout << "ChildCityObjectsCount : " << cc << std::endl;
        for (unsigned int i = 0; i < cc; i++) {
            const auto& target_object = city_model.getRootCityObject(h).getChildCityObject(i);
            ofs_ << "g " << target_object.getId() << std::endl;
            std::cout << "ID : " << target_object.getId() << std::endl;
            const auto gc = target_object.getGeometriesCount();
            for (unsigned int j = 0; j < gc; j++) {
                const auto pc = target_object.getGeometry(j).getPolygonsCount();
                for (unsigned int k = 0; k < pc; k++) {
                    cntv = writeVertices(target_object.getGeometry(j).getPolygon(k)->getVertices());

                    const auto citygmlTex = target_object.getGeometry(j).getPolygon(k)->getTextureFor("rgbTexture");
                    bool tex_flg = false;
                    cntt = 0;
                    if(citygmlTex){ 
                        tex_flg = true;
                        std::string tex_path = citygmlTex->getUrl();
                        size_t path_i = tex_path.find_last_of("/") + 1;
                        size_t ext_i = tex_path.find_last_of(".");
                        std::string mat_name = tex_path.substr(path_i, ext_i - path_i);
                        cntt = writeUVs(target_object.getGeometry(j).getPolygon(k)->getTexCoordsForTheme("rgbTexture", true));
                        ofs_ << "usemtl " << mat_name << std::endl;
                        
                        bool newmat_flg = true;
                        for (const auto& item : mat_list) {
                            if (item == mat_name) {
                                newmat_flg = false;
                                break;
                            }
                        }
                        if (newmat_flg) {
                            ofs_mat << "newmtl " << mat_name << std::endl;
                            ofs_mat << "map_Kd ./" << tex_path << std::endl << std::endl;
                            mat_list.push_back(mat_name);
                        }
                    }
                    
                    writeIndices(target_object.getGeometry(j).getPolygon(k)->getIndices(), v_cnt, t_cnt, tex_flg);
                    v_cnt += cntv;
                    t_cnt += cntt;
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
        ofs_ << "v " << xyz[0] << " " << xyz[1] << " " << xyz[2] << std::endl;
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
        }
        else {
            ofs_ << *itr + 1 + ix_offset << " ";
        }
        i++;
        if (i == 3) {
            i = 0;
            ofs_ << std::endl;
        }
    }
}
