#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iomanip>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/geometry.h>
#include <citygml/polygon.h>

#include "obj_writer.h"

void ObjWriter::write(const std::string& file_path, const citygml::CityModel& city_model) {
    
    unsigned int v_cnt = 0, cnt;

    ofs_ = std::ofstream(file_path);
    if (!ofs_.is_open()) {
        throw std::runtime_error(std::string("Failed to open : ") + file_path);
    }

    ofs_ << std::fixed << std::setprecision(8);//有効桁数8桁でOK？

    //とりあえず8つの建物のうちの先頭の建物ID「13111-bldg-147301」のみ対象
    std::cout << "建物ID : " << city_model.getRootCityObject(0).getAttribute("建物ID") << std::endl;
    std::cout << "NumRootCityObjects : " << city_model.getNumRootCityObjects() << std::endl;

    int cc = city_model.getRootCityObject(0).getChildCityObjectsCount();
    std::cout << "ChildCityObjectsCount : " << cc << std::endl;
    for (int i = 0; i < cc; i++) {
        const auto& target_object = city_model.getRootCityObject(0).getChildCityObject(i);
        ofs_ << "g " << target_object.getId() << std::endl;
        int gc = target_object.getGeometriesCount();
        std::cout << "GeometriesCount : " << gc << std::endl;
        for (int j = 0; j < gc; j++) {
            int pc = target_object.getGeometry(j).getPolygonsCount();
            std::cout << "PolygonsCount : " << pc << std::endl;
            for (int k = 0; k < pc; k++) {
                cnt = writeVertices(target_object.getGeometry(j).getPolygon(k)->getVertices());
                writeIndices(target_object.getGeometry(j).getPolygon(k)->getIndices(), v_cnt);
                v_cnt += cnt;
            }
        }
    }

    ofs_.close();
}

unsigned int ObjWriter::writeVertices(const std::vector<TVec3d>& vertices) {
    int cnt = 0;
    std::for_each(vertices.cbegin(), vertices.cend(), [&](const TVec3d& v) {
        ofs_ << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
        cnt++;
    });
    return cnt;
}

void ObjWriter::writeIndices(const std::vector<unsigned int>& indices, unsigned int ix_offset) {
    int i = 0;
    
    for (auto itr = indices.begin(); itr != indices.end(); itr++) {
        if(i == 0) ofs_ << "f ";
        ofs_ << *itr + 1 + ix_offset << " ";
        i++;
        if (i == 3) {
            i = 0;
            ofs_ << std::endl;
        }
    }
}
