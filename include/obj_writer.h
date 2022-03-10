#pragma once
#include <string>
#include <fstream>

#include <citygml/citygml.h>

class ObjWriter {
public:
    ObjWriter() : ofs_() {
    }

    void write(const std::string& obj_file_path, const citygml::CityModel& city_model, const std::string& gml_file_path);
    void setMergeMeshFlg(bool value);

private:
    unsigned int writeVertices(const std::vector<TVec3d>& vertices);
    void writeIndices(const std::vector<unsigned int>& indices, unsigned int ix_offset, unsigned int tx_offset, bool tex_flg);
    unsigned int writeUVs(const std::vector<TVec2f>& uvs);
    void writeMaterial(const std::string& tex_path);

    std::ofstream ofs_;
    std::ofstream ofs_mat_;
    std::string gml_file_path_, obj_file_path_;
    std::vector<std::string> mat_list_;
    bool merge_mesh_flg_ = false;
};
