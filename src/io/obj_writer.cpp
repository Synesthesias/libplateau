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

#include <plateau/io/obj_writer.h>
#include "polar_to_plane_cartesian.h"

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
        std::ostringstream oss;
        oss << "newmtl " << name << std::endl;
        oss << "map_Kd ./" << texture_path << std::endl;
        return oss.str();
    }

    void copyTexture(const std::string& gml_path, const std::string& obj_path, const std::string& texture_url) {
        const auto src_path = fs::u8path(gml_path).parent_path().append(fs::u8path(texture_url).string());
        const auto dst_path = fs::u8path(obj_path).parent_path().append(fs::u8path(texture_url).string());

        create_directory(dst_path.parent_path());

        constexpr auto copy_options =
                fs::copy_options::skip_existing;
        copy(src_path, dst_path, copy_options);
    }

    TVec3d convertAxes(const TVec3d& position, const CoordinateSystem axes) {
        TVec3d converted_position = position;
        switch (axes) {
            case CoordinateSystem::ENU:
                return converted_position;
            case CoordinateSystem::WUN:
                converted_position.x = -position.x;
                converted_position.y = position.z;
                converted_position.z = position.y;
                return converted_position;
            case CoordinateSystem::NWU:
                converted_position.x = position.y;
                converted_position.y = -position.x;
                converted_position.z = position.z;
                return converted_position;
            case CoordinateSystem::EUN:
                converted_position.x = position.x;
                converted_position.y = position.z;
                converted_position.z = position.y;
                return converted_position;
            default:
                throw std::out_of_range("Invalid argument");
        }
    }

    unsigned getMaxLOD(const citygml::CityObject& object) {
        unsigned max_lod = 0;
        for (unsigned i = 0; i < object.getGeometriesCount(); i++) {
            max_lod = std::max(max_lod, object.getGeometry(i).getLOD());
        }
        return max_lod;
    }

    bool hasAnyGeometry(const citygml::CityObject& object, unsigned lod) {
        for (unsigned i = 0; i < object.getGeometriesCount(); i++) {
            if (lod == object.getGeometry(i).getLOD()) {
                return true;
            }
        }
        return false;
    }
}

bool ObjWriter::write(const std::string& obj_file_path, const std::string& gml_file_path,
                      const citygml::CityModel& city_model, MeshConvertOptions options, unsigned lod,
                      std::shared_ptr<PlateauDllLogger> logger) {
    // 内部保持するロガー用意。引数指定されていない場合は関数スコープ
    const auto& local_logger = logger == nullptr
                               ? std::make_shared<PlateauDllLogger>()
                               : logger;
    dll_logger_ = local_logger;

    dll_logger_.lock()->log(DllLogLevel::LL_INFO,
                            "Start conversion.\ngml path = " + gml_file_path + "\nobj path = " + obj_file_path);

    // 内部状態初期化
    options_ = options;
    v_offset_ = 0;
    uv_offset_ = 0;
    required_materials_.clear();

    writeObj(obj_file_path, city_model, lod);

    // 中身が空ならOBJ削除
    if (v_offset_ == 0) {
        dll_logger_.lock()->log(DllLogLevel::LL_INFO, "No vertex generated. Deleting output obj.");
        fs::remove(obj_file_path);
        return false;
    }

    if (options_.export_appearance) {
        // テクスチャファイルコピー
        for (const auto& [_, texture]: required_materials_) {
            const auto& texture_url = texture->getUrl();
            copyTexture(gml_file_path, obj_file_path, texture_url);
        }

        writeMtl(obj_file_path);
    }

    dll_logger_.lock()->log(DllLogLevel::LL_INFO, "Conversion succeeded");

    return true;
}

void ObjWriter::writeObj(const std::string& obj_file_path, const citygml::CityModel& city_model, unsigned lod) {
    auto ofs = std::ofstream(obj_file_path);
    if (!ofs.is_open()) {
        dll_logger_.lock()->throwException(std::string("Failed to open stream of obj path : ") + obj_file_path);
    }
    ofs << std::fixed << std::setprecision(6);

    // MTL参照
    const auto mtl_file_name = fs::u8path(obj_file_path).filename().replace_extension(".mtl").string();
    ofs << "mtllib " << mtl_file_name << std::endl;

    // エリア単位の場合拡張子無しファイル名がメッシュ名になります。
    if (options_.mesh_granularity == MeshGranularity::PerCityModelArea) {
        startMeshGroup(ofs, fs::u8path(obj_file_path).filename().replace_extension().string());
    }

    for (const auto& root_object: city_model.getRootCityObjects()) {
        writeCityObjectRecursive(ofs, *root_object, lod);
    }
}

void ObjWriter::writeCityObjectRecursive(std::ofstream& ofs, const citygml::CityObject& target_object, unsigned lod) {
    writeCityObject(ofs, target_object, lod);

    for (unsigned i = 0; i < target_object.getChildCityObjectsCount(); i++) {
        const auto& child = target_object.getChildCityObject(i);
        writeCityObjectRecursive(ofs, child, lod);
    }
}

void ObjWriter::writeCityObject(std::ofstream& ofs, const citygml::CityObject& target_object, unsigned lod) {
    // 最大LODのみ出力する場合は最大LOD以外のジオメトリをスキップ
    const auto max_lod = std::min(options_.max_lod, getMaxLOD(target_object));
    const auto is_lower_lod = lod != max_lod;
    if (!options_.export_lower_lod && is_lower_lod) {
        return;
    }

    // LODがオブジェクトに含まれない場合はスキップ
    if (!hasAnyGeometry(target_object, lod)) {
        return;
    }

    // 該当する地物ではない場合はスキップ
    const auto is_primary = PrimaryCityObjectTypes::isPrimary(target_object.getType());
    auto should_write = false;
    auto should_start_new_group = false;
    switch (options_.mesh_granularity) {
        case MeshGranularity::PerCityModelArea:
            should_write = true;
            should_start_new_group = false;
            break;
        case MeshGranularity::PerPrimaryFeatureObject:
            should_write = is_primary;
            should_start_new_group = is_primary;
            break;
        case MeshGranularity::PerAtomicFeatureObject:
            if (lod <= 1) {
                // LOD1以下は階層構造を持たないため常に書き出し&分解
                should_write = true;
                should_start_new_group = true;
            } else {
                // 建築物のLOD2,3については別オブジェクトにも２重にジオメトリが存在するため除外
                should_write =
                        target_object.getType() != CityObject::CityObjectsType::COT_Building &&
                        target_object.getType() != CityObject::CityObjectsType::COT_BuildingPart &&
                        target_object.getType() != CityObject::CityObjectsType::COT_BuildingInstallation;

                // TODO: 建築物については冗長な空のグループができるため削除必要
                should_start_new_group = true;
            }
    }

    if (!should_write) {
        return;
    }

    if (should_start_new_group) {
        std::stringstream ss;
        ss << "LOD" << lod << "_" << target_object.getId();
        startMeshGroup(ofs, ss.str());
    }

    for (unsigned i = 0; i < target_object.getGeometriesCount(); i++) {
        const auto target_lod = target_object.getGeometry(i).getLOD();
        if (target_lod == lod)
            writeGeometry(ofs, target_object.getGeometry(i));
    }
}

void ObjWriter::writeGeometry(std::ofstream& ofs, const citygml::Geometry& target_geometry) {
    for (unsigned int i = 0; i < target_geometry.getPolygonsCount(); i++) {
        const auto polygon = target_geometry.getPolygon(i);
        const auto& vertices = polygon->getVertices();
        writeVertices(ofs, vertices);

        const auto uvs = polygon->getTexCoordsForTheme("rgbTexture", true);
        writeUVs(ofs, uvs);

        const auto texture = polygon->getTextureFor("rgbTexture");
        writeMaterialReference(ofs, texture);

        const auto& indices = polygon->getIndices();
        if (uvs.empty()) {
            writeIndices(ofs, indices);
        } else {
            writeIndicesWithUV(ofs, indices);
        }

        v_offset_ += vertices.size();
        uv_offset_ += uvs.size();
    }

    for (unsigned int i = 0; i < target_geometry.getGeometriesCount(); i++) {
        const auto& child_geometry = target_geometry.getGeometry(i);
        writeGeometry(ofs, child_geometry);
    }
}

void ObjWriter::writeVertices(std::ofstream& ofs, const std::vector<TVec3d>& vertices) {
    for (TVec3d vertex: vertices) {
        polar_to_plane_cartesian().convert(vertex, 9);
        vertex = convertPosition(vertex, options_.reference_point, options_.mesh_axes, options_.unit_scale);
        ofs << generateVertex(vertex);
    }
}

void ObjWriter::writeUVs(std::ofstream& ofs, const std::vector<TVec2f>& uvs) {
    for (const auto& uv: uvs) {
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

void ObjWriter::writeMaterialReference(std::ofstream& ofs, const std::shared_ptr<const Texture>& texture) {
    if (texture == nullptr) {
        applyDefaultMaterial(ofs);
        return;
    }

    // マテリアル名はテクスチャファイル名(拡張子抜き)
    const auto& texture_url = texture->getUrl();
    const auto material_name = fs::u8path(texture_url).filename().replace_extension().u8string();

    applyMaterial(ofs, material_name);

    const bool material_exists = required_materials_.find(material_name) != required_materials_.end();
    if (!material_exists) {
        required_materials_[material_name] = texture;
    }
}

void ObjWriter::writeMtl(const std::string& obj_file_path) {
    const auto mtl_file_path = fs::u8path(obj_file_path).replace_extension(".mtl").string();
    auto mtl_ofs = std::ofstream(mtl_file_path);
    if (!mtl_ofs.is_open()) {
        dll_logger_.lock()->throwException(std::string("Failed to open mtl file: ") + mtl_file_path);
    }

    mtl_ofs << generateDefaultMtl();
    for (auto& [material_name, texture]: required_materials_) {
        const auto& texture_url = texture->getUrl();
        mtl_ofs << generateMtl(material_name, texture_url);
    }
}

TVec3d ObjWriter::convertPosition(const TVec3d& position, const TVec3d& reference_point, const CoordinateSystem axes,
                                  float unit_scale) {
    const auto referenced_position = position - reference_point;
    const auto scaled_position = referenced_position / unit_scale;
    return convertAxes(scaled_position, axes);
}
