#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <filesystem>

#include <citygml/citymodel.h>
#include <citygml/geometry.h>
#include <citygml/polygon.h>
#include <citygml/texture.h>

#include <plateau/polygon_mesh/primary_city_object_types.h>

#include <plateau/mesh_writer/obj_writer.h>

namespace fs = std::filesystem;
using namespace citygml;

namespace {
    void startMeshGroup(std::ofstream& obj_ofs, const std::string& name) {
        obj_ofs << "g " << name << std::endl;
    }

    void applyMaterial(std::ofstream& obj_ofs, const std::string& name) {
        obj_ofs << "usemtl " << name << std::endl;
    }

    void applyDefaultMaterial(std::ofstream& obj_ofs) {
        applyMaterial(obj_ofs, "Default-Material");
    }

    std::string generateVertex(const TVec3d& vertex) {
        std::ostringstream oss;
        oss << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
        return oss.str();
    }

    std::string generateFace(unsigned i0, unsigned i1, unsigned i2) {
        std::ostringstream oss;
        oss << "f " << i0 << " " << i1 << " " << i2 << std::endl;
        return oss.str();
    }

    std::string generateFaceWithUV(unsigned i0, unsigned i1, unsigned i2, unsigned u0, unsigned u1, unsigned u2) {
        std::ostringstream oss;
        oss << "f ";
        oss << i0 << "/" << u0 << " ";
        oss << i1 << "/" << u1 << " ";
        oss << i2 << "/" << u2 << " " << std::endl;
        return oss.str();
    }

    std::string generateDefaultMtl() {
        std::ostringstream oss;
        oss << "newmtl Default-Material" << std::endl;
        oss << "Kd 0.5 0.5 0.5" << std::endl << std::endl;
        return oss.str();
    }

    std::string generateMtl(const std::string& name, const std::string& texture_path) {
        auto src_path = fs::u8path(texture_path);
        auto t_path = (src_path.parent_path().filename() / src_path.filename()).u8string();
        std::replace(t_path.begin(), t_path.end(), '\\', '/');

        std::ostringstream oss;
        oss << "newmtl " << name << std::endl;
        oss << "map_Kd ./" << t_path << std::endl;
        return oss.str();
    }

    void copyTexture(const std::string& obj_path, const std::string& texture_url) {
        auto src_path = fs::u8path(texture_url);
        auto t_path = src_path.filename();
        auto dst_path = fs::u8path(obj_path).parent_path();
        dst_path /= src_path.parent_path().filename();
        create_directories(dst_path);
        dst_path /= t_path;

        constexpr auto copy_options = fs::copy_options::skip_existing;
        copy(src_path, dst_path, copy_options);
    }
}

namespace plateau::meshWriter {
    bool ObjWriter::write(const std::string& obj_file_path, const plateau::polygonMesh::Model& model) {

        // 内部状態初期化
        v_offset_ = 0;
        uv_offset_ = 0;
        required_materials_.clear();

        std::filesystem::path path = obj_file_path;
        if (path.is_relative()) {
            auto pathCurrent = std::filesystem::current_path();
            pathCurrent /= path;
            pathCurrent.swap(path);
        }

        writeObj(path.u8string(), model);

        // テクスチャファイルコピー
        for (const auto& [_, texture_url] : required_materials_) {
            copyTexture(fs::absolute(path).u8string(), texture_url);
        }

        writeMtl(obj_file_path);

        return true;
    }

    void ObjWriter::writeObj(const std::string& obj_file_path, const plateau::polygonMesh::Model& model) {
        auto ofs = std::ofstream(obj_file_path);
        if (!ofs.is_open()) {
            throw std::string("Failed to open stream of obj path : ") + obj_file_path;
        }
        ofs << std::fixed << std::setprecision(6);

        // MTL参照
        const auto mtl_file_name = fs::u8path(obj_file_path).filename().replace_extension(".mtl").string();
        ofs << "mtllib " << mtl_file_name << std::endl;

        auto& root_node = model.getRootNodeAt(0);
        auto num_root_child = root_node.getChildCount();
        if (0 < num_root_child) {
            for (int lod = 0; lod < num_root_child; lod++) {
                auto& lod_node = root_node.getChildAt(lod);
                writeCityObjectRecursive(ofs, lod_node);
            }
        }
    }

    void ObjWriter::writeCityObjectRecursive(std::ofstream& ofs, const plateau::polygonMesh::Node& node) {
        writeCityObject(ofs, node);

        auto num_child = node.getChildCount();
        for (int i = 0; i < num_child; i++) {
            writeCityObjectRecursive(ofs, node.getChildAt(i));
        }
    }

    void ObjWriter::writeCityObject(std::ofstream& ofs, const plateau::polygonMesh::Node& node) {

        auto node_name = node.getName();

        const auto& mesh = node.getMesh();
        if (mesh.has_value()) {

            const auto& sub_meshes = mesh.value().getSubMeshes();
            if (!sub_meshes.empty()) {
                std::stringstream ss;
                ss << node_name;
                startMeshGroup(ofs, ss.str());

                const auto& vertices = mesh.value().getVertices();
                const auto& all_indices = mesh.value().getIndices();
                const auto& uvs = mesh.value().getUV1();

                writeVertices(ofs, vertices);

                if (0 < uvs.size()) {
                    std::vector<TVec2f> texcoords; // TODO texcoords、使われていないのでは？
                    for (const auto& uv : uvs) {
                        auto t = uv;
                        t.y = 1.0 - uv.y;
                        texcoords.push_back(t);
                    }
                    writeUVs(ofs, uvs);
                }

                for (auto& sub_mesh : sub_meshes) {
                    auto st = sub_mesh.getStartIndex();
                    auto ed = sub_mesh.getEndIndex();
                    std::vector<unsigned int> indices(all_indices.begin() + st, all_indices.begin() + ed + 1);

                    auto texUrl = sub_mesh.getTexturePath();
                    std::replace(texUrl.begin(), texUrl.end(), '\\', '/');
                    writeMaterialReference(ofs, texUrl);

                    if (!texUrl.empty()) {
                        writeIndicesWithUV(ofs, indices);
                    } else {
                        writeIndices(ofs, indices);
                    }
                }
                v_offset_ += vertices.size();
                uv_offset_ += uvs.size();
            }
        }
    }

    void ObjWriter::writeVertices(std::ofstream& ofs, const std::vector<TVec3d>& vertices) {
        for (TVec3d vertex : vertices) {
            ofs << generateVertex(vertex);
        }
    }

    void ObjWriter::writeUVs(std::ofstream& ofs, const std::vector<TVec2f>& uvs) {
        for (const auto& uv : uvs) {
            ofs << "vt " << uv.x << " " << uv.y << std::endl;
        }
    }

    void ObjWriter::writeIndices(std::ofstream& ofs, const std::vector<unsigned int>& indices) {
        unsigned face[3];
        for (unsigned i = 0; i < indices.size(); i++) {
            face[i % 3] = indices[i] + v_offset_ + 1;

            if (i % 3 < 2) {
                continue;
            }

            ofs << generateFace(face[0], face[1], face[2]);
        }
    }

    void ObjWriter::writeIndicesWithUV(std::ofstream& ofs, const std::vector<unsigned int>& indices) {
        unsigned face[3] = {};
        unsigned uv_face[3] = {};
        for (unsigned i = 0; i < indices.size(); i++) {
            face[i % 3] = indices[i] + v_offset_ + 1;
            uv_face[i % 3] = indices[i] + uv_offset_ + 1;

            if (i % 3 < 2) {
                continue;
            }

            ofs << generateFaceWithUV(
                    face[0], face[1], face[2],
                    uv_face[0], uv_face[1], uv_face[2]);
        }
    }

    void ObjWriter::writeMaterialReference(std::ofstream& ofs, const std::string texUrl) {
        if (texUrl == "") {
            applyDefaultMaterial(ofs);
            return;
        }

        // マテリアル名はテクスチャファイル名(拡張子抜き)
        const auto material_name = fs::u8path(texUrl).filename().replace_extension().u8string();

        applyMaterial(ofs, material_name);

        const bool material_exists = required_materials_.find(material_name) != required_materials_.end();
        if (!material_exists) {
            required_materials_[material_name] = texUrl;
        }
    }

    void ObjWriter::writeMtl(const std::string& obj_file_path) {
        const auto mtl_file_path = fs::u8path(obj_file_path).replace_extension(".mtl").string();
        auto mtl_ofs = std::ofstream(mtl_file_path);
        if (!mtl_ofs.is_open()) {
            throw std::string("Failed to open mtl file: ") + mtl_file_path;
        }

        mtl_ofs << generateDefaultMtl();
        for (auto& [material_name, texture_url] : required_materials_) {
            mtl_ofs << generateMtl(material_name, texture_url);
        }
    }
}
