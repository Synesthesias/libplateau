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
    unsigned int writeVertices(const std::vector<TVec3d>& vertices);
    void writeIndices(const std::vector<unsigned int>& indices, unsigned int ix_offset, unsigned int tx_offset, bool tex_flg);
    unsigned int writeUVs(const std::vector<TVec2f>& uvs);

    std::ofstream ofs_;
};
