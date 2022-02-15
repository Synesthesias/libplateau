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

    ofs_ << std::fixed << std::setprecision(8);

    {
        const auto& target_object = city_model.getRootCityObject(0).getChildCityObject(0);
        ofs_ << "g " << target_object.getId() << std::endl;
        cnt = writeVertices(target_object.getGeometry(0).getPolygon(0)->getVertices());
        writeIndices(target_object.getGeometry(0).getPolygon(0)->getIndices(), v_cnt);
        v_cnt += cnt;
    }
    std::cout << "city_model.getNumRootCityObjects() : " <<  city_model.getNumRootCityObjects() << std::endl;
    std::cout << "city_model.getRootCityObject(0).getChildCityObjectsCount() : " << city_model.getRootCityObject(0).getChildCityObjectsCount() << std::endl;
    std::cout << "target_object.getGeometriesCount() : " << city_model.getRootCityObject(0).getChildCityObject(0).getGeometriesCount() << std::endl;
    std::cout << "target_object.getGeometry(0).getPolygonsCount() : " << city_model.getRootCityObject(0).getChildCityObject(0).getGeometry(0).getPolygonsCount() << std::endl;
    {
        const auto& target_object = city_model.getRootCityObject(0).getChildCityObject(1);
        ofs_ << "g " << target_object.getId() << std::endl;
        cnt = writeVertices(target_object.getGeometry(0).getPolygon(0)->getVertices());
        writeIndices(target_object.getGeometry(0).getPolygon(0)->getIndices(), v_cnt);
        v_cnt += cnt;
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
