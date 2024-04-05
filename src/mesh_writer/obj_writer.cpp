#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <filesystem>

#include <citygml/citymodel.h>

#include <plateau/mesh_writer/obj_writer.h>
#include <cassert>
#include <fstream>

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
        required_materials_.clear();

        std::filesystem::path path = std::filesystem::u8path(obj_file_path);
        if (path.is_relative()) {
            auto current_path = std::filesystem::current_path();
            current_path /= path;
            current_path.swap(path);
        }

        for (size_t i = 0; i < model.getRootNodeCount(); ++i) {
            // 内部状態初期化
            v_offset_ = 0;
            uv_offset_ = 0;

            auto& root_node = model.getRootNodeAt(i);

            // ファイル名設定
            std::stringstream oss;
            oss << "_" << root_node.getName() << ".obj";
            const auto filename_without_ext = fs::u8path(obj_file_path).filename().replace_extension("").u8string();
            const auto& file_path =
                fs::u8path(obj_file_path)
                .parent_path()
                .append(filename_without_ext + oss.str())
                .u8string();

            auto transform_stack = TransformStack();
            transform_stack.push(Transform(root_node.getLocalPosition()));
            writeObj(file_path, root_node, transform_stack);

            writeMtl(file_path);
        }

        // テクスチャファイルコピー
        for (const auto& [_, texture_url] : required_materials_) {
            copyTexture(fs::absolute(path).u8string(), texture_url);
        }

        return true;
    }

    void ObjWriter::writeObj(const std::string& obj_file_path, const plateau::polygonMesh::Node& node,
                             TransformStack& transform_stack) {
        auto ofs = std::ofstream(fs::u8path(obj_file_path));
        if (!ofs.is_open()) {
            throw std::runtime_error("Failed to open stream of obj path : " + obj_file_path);
        }
        ofs << std::fixed << std::setprecision(6);

        // MTL参照
        const auto mtl_file_name = fs::u8path(obj_file_path).filename().replace_extension(".mtl").string();
        ofs << "mtllib " << mtl_file_name << std::endl;

        writeCityObjectRecursive(ofs, node, transform_stack);
    }

    void ObjWriter::writeCityObjectRecursive(std::ofstream& ofs, const plateau::polygonMesh::Node& node, TransformStack& transform_stack) {
        transform_stack.push(Transform(node.getLocalPosition()));
        writeCityObject(ofs, node, transform_stack);

        for (size_t i = 0; i < node.getChildCount(); i++) {
            writeCityObjectRecursive(ofs, node.getChildAt(i), transform_stack);
        }
        transform_stack.pop();
    }

    void ObjWriter::writeCityObject(std::ofstream& ofs, const plateau::polygonMesh::Node& node, TransformStack& transform_stack) {

        const auto& node_name = node.getName();

        const auto mesh = node.getMesh();
        if (mesh != nullptr) {
            const auto& sub_meshes = mesh->getSubMeshes();
            if (!sub_meshes.empty()) {
                std::stringstream ss;
                ss << node_name;
                startMeshGroup(ofs, ss.str());

                const auto& vertices = mesh->getVertices();
                const auto& all_indices = mesh->getIndices();
                const auto& uvs = mesh->getUV1();

                assert(all_indices.size() % 3 == 0);

                writeVertices(ofs, vertices, transform_stack);

                if (!uvs.empty()) {
                    std::vector<TVec2f> texcoords; // TODO texcoords、使われていないのでは？
                    for (const auto& uv : uvs) {
                        auto t = uv;
                        t.y = 1.0f - uv.y;
                        texcoords.push_back(t);
                    }
                    writeUVs(ofs, uvs);
                }

                for (auto& sub_mesh : sub_meshes) {
                    auto st = sub_mesh.getStartIndex();
                    auto ed = sub_mesh.getEndIndex();
                    std::vector<unsigned int> indices(all_indices.begin() + (long long)st, all_indices.begin() + (long long)ed + 1);
                    assert(indices.size() % 3 == 0);

                    auto texUrl = sub_mesh.getTexturePath();
                    std::replace(texUrl.begin(), texUrl.end(), '\\', '/');
                    writeMaterialReference(ofs, texUrl);

                    // UV番号を明記する記法と省略する記法が混在すると Blender にインポートしたときにUVがずれるので
                    // テクスチャがなくともUVは記載します。
                    writeIndicesWithUV(ofs, indices);
                }
                v_offset_ += vertices.size();
                uv_offset_ += uvs.size();
            }
        }
    }

    void ObjWriter::writeVertices(std::ofstream& ofs, const std::vector<TVec3d>& vertices, TransformStack& transform_stack) {
        auto position_offset = transform_stack.getSumOfPosition();
        for (const auto& vertex : vertices) {
            ofs << generateVertex(vertex + position_offset);
        }
    }

    void ObjWriter::writeUVs(std::ofstream& ofs, const std::vector<TVec2f>& uvs) {
        for (const auto& uv : uvs) {
            ofs << "vt " << uv.x << " " << uv.y << std::endl;
        }
    }

    void ObjWriter::writeIndicesWithUV(std::ofstream& ofs, const std::vector<unsigned int>& indices) const {
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

    void ObjWriter::writeMaterialReference(std::ofstream& ofs, const std::string& texUrl) {
        if (texUrl.empty()) {
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
        const auto mtl_file_path = fs::u8path(obj_file_path).replace_extension(".mtl");
        auto mtl_ofs = std::ofstream(mtl_file_path);
        if (!mtl_ofs.is_open()) {
            throw std::runtime_error("Failed to open mtl file: " + mtl_file_path.u8string());
        }

        mtl_ofs << generateDefaultMtl();
        for (auto& [material_name, texture_url] : required_materials_) {
            mtl_ofs << generateMtl(material_name, texture_url);
        }
    }
}
