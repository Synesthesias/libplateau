﻿#pragma once

#include <string>
#include <fstream>

#include <citygml/citygml.h>
#include <libplateau_api.h>
#include <plateau_dll_logger.h>

#include <plateau/polygon_mesh/mesh_extractor.h>

namespace plateau::meshWriter {
    class LIBPLATEAU_EXPORT ObjWriter {
    public:
        ObjWriter() :
            v_offset_(0), uv_offset_(0), required_materials_() {
        }

        bool write(const std::string& obj_file_path, const plateau::polygonMesh::Model& model);

    private:
        // OBJ書き出し
        void writeObj(const std::string& obj_file_path_utf8, const plateau::polygonMesh::Model& model);
        void writeCityObjectRecursive(std::ofstream& ofs, const plateau::polygonMesh::Node& node);
        void writeCityObject(std::ofstream& ofs, const plateau::polygonMesh::Node& node);
        void writeVertices(std::ofstream& ofs, const std::vector<TVec3d>& vertices);
        void writeIndices(std::ofstream& ofs, const std::vector<unsigned int>& indices);
        void writeIndicesWithUV(std::ofstream& ofs, const std::vector<unsigned int>& indices);
        void writeUVs(std::ofstream& ofs, const std::vector<TVec2f>& uvs);
        void writeMaterialReference(std::ofstream& ofs, const std::string texUrl);

        // MTL書き出し
        void writeMtl(const std::string& obj_file_path);


        std::map<std::string, std::string> required_materials_;
        unsigned v_offset_, uv_offset_;

    };
}
