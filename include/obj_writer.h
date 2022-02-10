#pragma once
#include <string>
#include <fstream>

#include <citygml/citygml.h>

class ObjWriter {
public:
    ObjWriter() : ofs_() {
    }

    void write(const std::string& file_path, const citygml::CityModel& city_model);

private:
    void writeVertices(const std::vector<TVec3d>& vertices);

    std::ofstream ofs_;
};
