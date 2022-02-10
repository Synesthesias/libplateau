#include <stdexcept>
#include <vector>
#include <algorithm>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/geometry.h>
#include <citygml/polygon.h>

#include "obj_writer.h"

void ObjWriter::write(const std::string& file_path, const citygml::CityModel& city_model) {
    // ˆê‚Â‚¾‚¯CityObject‚Ì’¸“_‚ð.obj‰»‚·‚éŽÀ‘•
    ofs_ = std::ofstream(file_path);
    if (!ofs_.is_open()) {
        throw std::runtime_error(std::string("Failed to open : ") + file_path);
    }

    const auto& target_object = city_model.getRootCityObject(0).getChildCityObject(0);
    ofs_ << "g " << target_object.getId() << std::endl;
    writeVertices(target_object.getGeometry(0).getPolygon(0)->getVertices());
    ofs_.close();
}

void ObjWriter::writeVertices(const std::vector<TVec3d>& vertices) {
    std::for_each(vertices.cbegin(), vertices.cend(), [&](const TVec3d& v) {
        ofs_ << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
    });
}
