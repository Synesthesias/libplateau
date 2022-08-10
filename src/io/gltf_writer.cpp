#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <filesystem>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/geometry.h>
#include <citygml/polygon.h>
#include <citygml/texture.h>

#include <plateau/io/primary_city_object_types.h>

#include "gltf_writer.h"
#include "polar_to_plane_cartesian.h"

#include <cassert>

namespace fs = std::filesystem;
namespace gltf = Microsoft::glTF;

namespace {
    // The glTF SDK is decoupled from all file I/O by the IStreamWriter (and IStreamReader)
    // interface(s) and the C++ stream-based I/O library. This allows the glTF SDK to be used in
    // sandboxed environments, such as WebAssembly modules and UWP apps, where any file I/O code
    // must be platform or use-case specific.
    class StreamWriter : public gltf::IStreamWriter {
    public:
        StreamWriter(fs::path pathBase) : m_pathBase(std::move(pathBase)) {
            assert(m_pathBase.has_root_path());
        }

        // Resolves the relative URIs of any external resources declared in the glTF manifest
        std::shared_ptr<std::ostream> GetOutputStream(const std::string& filename) const override {
            // In order to construct a valid stream:
            // 1. The filename argument will be encoded as UTF-8 so use filesystem::u8path to
            //    correctly construct a path instance.
            // 2. Generate an absolute path by concatenating m_pathBase with the specified filename
            //    path. The filesystem::operator/ uses the platform's preferred directory separator
            //    if appropriate.
            // 3. Always open the file stream in binary mode. The glTF SDK will handle any text
            //    encoding issues for us.
            auto streamPath = m_pathBase / fs::u8path(filename);
            auto stream = std::make_shared<std::ofstream>(streamPath, std::ios_base::binary);

            // Check if the stream has no errors and is ready for I/O operations
            if (!stream || !(*stream)) {
                throw std::runtime_error("Unable to create a valid output stream for uri: " + filename);
            }

            return stream;
        }

    private:
        fs::path m_pathBase;
    };

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

    TVec3d convertAxes(const TVec3d& position, const AxesConversion axes) {
        TVec3d converted_position = position;
        switch (axes) {
        case AxesConversion::ENU:
            return converted_position;
        case AxesConversion::WUN:
            converted_position.x = -position.x;
            converted_position.y = position.z;
            converted_position.z = position.y;
            return converted_position;
        case AxesConversion::NWU:
            converted_position.x = position.y;
            converted_position.y = -position.x;
            converted_position.z = position.z;
            return converted_position;
        default:
            throw std::out_of_range("Invalid argument");
        }
    }

    TVec3d convertPosition(const TVec3d& position, const TVec3d& reference_point, const AxesConversion axes, float unit_scale) {
        const auto referenced_position = position - reference_point;
        const auto scaled_position = referenced_position / unit_scale;
        return convertAxes(scaled_position, axes);
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

void CreateTriangleResources(gltf::Document& document, gltf::BufferBuilder& bufferBuilder, std::string& accessorIdIndices, std::string& accessorIdPositions) {
    // Create all the resource data (e.g. triangle indices and
    // vertex positions) that will be written to the binary buffer
    const char* bufferId = nullptr;

    // Specify the 'special' GLB buffer ID. This informs the GLBResourceWriter that it should use
    // the GLB container's binary chunk (usually the desired buffer location when creating GLBs)
    if (dynamic_cast<const gltf::GLBResourceWriter*>(&bufferBuilder.GetResourceWriter())) {
        bufferId = gltf::GLB_BUFFER_ID;
    }

    // Create a Buffer - it will be the 'current' Buffer that all the BufferViews
    // created by this BufferBuilder will automatically reference
    bufferBuilder.AddBuffer(bufferId);

    // Create a BufferView with a target of ELEMENT_ARRAY_BUFFER (as it will reference index
    // data) - it will be the 'current' BufferView that all the Accessors created by this
    // BufferBuilder will automatically reference
    bufferBuilder.AddBufferView(gltf::BufferViewTarget::ELEMENT_ARRAY_BUFFER);

    // Add an Accessor for the indices
    std::vector<uint16_t> indices = {
        0U, 1U, 2U
    };

    // Copy the Accessor's id - subsequent calls to AddAccessor may invalidate the returned reference
    accessorIdIndices = bufferBuilder.AddAccessor(indices, { gltf::TYPE_SCALAR, gltf::COMPONENT_UNSIGNED_SHORT }).id;

    // Create a BufferView with target ARRAY_BUFFER (as it will reference vertex attribute data)
    bufferBuilder.AddBufferView(gltf::BufferViewTarget::ARRAY_BUFFER);

    // Add an Accessor for the positions
    std::vector<float> positions = {
        0.0f, 0.0f, 0.0f, // Vertex 0
        1.0f, 0.0f, 0.0f, // Vertex 1
        0.0f, 1.0f, 0.0f  // Vertex 2
    };

    std::vector<float> minValues(3U, std::numeric_limits<float>::max());
    std::vector<float> maxValues(3U, std::numeric_limits<float>::lowest());

    const size_t positionCount = positions.size();

    // Accessor min/max properties must be set for vertex position data so calculate them here
    for (size_t i = 0U, j = 0U; i < positionCount; ++i, j = (i % 3U)) {
        minValues[j] = std::min(positions[i], minValues[j]);
        maxValues[j] = std::max(positions[i], maxValues[j]);
    }

    accessorIdPositions = bufferBuilder.AddAccessor(positions, { gltf::TYPE_VEC3, gltf::COMPONENT_FLOAT, false, std::move(minValues), std::move(maxValues) }).id;

    // Add all of the Buffers, BufferViews and Accessors that were created using BufferBuilder to
    // the Document. Note that after this point, no further calls should be made to BufferBuilder
    bufferBuilder.Output(document);
}

void CreateTriangleEntities(gltf::Document& document, const std::string& accessorIdIndices, const std::string& accessorIdPositions) {
    // Create a very simple glTF Document with the following hierarchy:
    //  Scene
    //     Node
    //       Mesh (Triangle)
    //         MeshPrimitive
    //           Material (Blue)
    // 
    // A Document can be constructed top-down or bottom up. However, if constructed top-down
    // then the IDs of child entities must be known in advance, which prevents using the glTF
    // SDK's automatic ID generation functionality.

    // Construct a Material
    gltf::Material material;
    material.metallicRoughness.baseColorFactor = gltf::Color4(0.0f, 0.0f, 1.0f, 1.0f);
    material.metallicRoughness.metallicFactor = 0.2f;
    material.metallicRoughness.roughnessFactor = 0.4f;
    material.doubleSided = true;

    // Add it to the Document and store the generated ID
    auto materialId = document.materials.Append(std::move(material), gltf::AppendIdPolicy::GenerateOnEmpty).id;

    // Construct a MeshPrimitive. Unlike most types in glTF, MeshPrimitives are direct children
    // of their parent Mesh entity rather than being children of the Document. This is why they
    // don't have an ID member.
    gltf::MeshPrimitive meshPrimitive;
    meshPrimitive.materialId = materialId;
    meshPrimitive.indicesAccessorId = accessorIdIndices;
    meshPrimitive.attributes[gltf::ACCESSOR_POSITION] = accessorIdPositions;

    // Construct a Mesh and add the MeshPrimitive as a child
    gltf::Mesh mesh;
    mesh.primitives.push_back(std::move(meshPrimitive));
    // Add it to the Document and store the generated ID
    auto meshId = document.meshes.Append(std::move(mesh), gltf::AppendIdPolicy::GenerateOnEmpty).id;

    // Construct a Node adding a reference to the Mesh
    gltf::Node node;
    node.meshId = meshId;
    // Add it to the Document and store the generated ID
    auto nodeId = document.nodes.Append(std::move(node), gltf::AppendIdPolicy::GenerateOnEmpty).id;

    // Construct a Scene
    gltf::Scene scene;
    scene.nodes.push_back(nodeId);
    // Add it to the Document, using a utility method that also sets the Scene as the Document's default
    document.SetDefaultScene(std::move(scene), gltf::AppendIdPolicy::GenerateOnEmpty);
}

bool GltfWriter::write(const std::string& gltf_file_path, const std::string& gml_file_path, const citygml::CityModel& city_model, MeshConvertOptions options, unsigned lod, std::shared_ptr<PlateauDllLogger> logger) {
    // 内部保持するロガー用意。引数指定されていない場合は関数スコープ
    const auto& local_logger = logger == nullptr
        ? std::make_shared<PlateauDllLogger>()
        : logger;
    dll_logger_ = local_logger;

    dll_logger_.lock()->log(DllLogLevel::LL_INFO, "Start conversion.\ngml path = " + gml_file_path + "\ngltf path = " + gltf_file_path);

    // 内部状態初期化
    options_ = options;
    v_offset_ = 0;
    uv_offset_ = 0;
    required_materials_.clear();

    std::filesystem::path path = gltf_file_path;
    if (path.is_relative()) {
        auto pathCurrent = std::filesystem::current_path();

        // Convert the relative path into an absolute path by appending the command line argument to the current path
        pathCurrent /= path;
        pathCurrent.swap(path);
    }

    // Pass the absolute path, without the filename, to the stream writer
    auto streamWriter = std::make_unique<StreamWriter>(path.parent_path());

    std::filesystem::path pathFile = path.filename();
    std::filesystem::path pathFileExt = pathFile.extension();

    auto MakePathExt = [](const std::string& ext) {
        return "." + ext;
    };

    std::unique_ptr < gltf::ResourceWriter > resourceWriter;

    // If the file has a '.gltf' extension then create a GLTFResourceWriter
    if (pathFileExt == MakePathExt(gltf::GLTF_EXTENSION)) {
        resourceWriter = std::make_unique<gltf::GLTFResourceWriter>(std::move(streamWriter));
    }

    // If the file has a '.glb' extension then create a GLBResourceWriter. This class derives
    // from GLTFResourceWriter and adds support for writing manifests to a GLB container's
    // JSON chunk and resource data to the binary chunk.
    if (pathFileExt == MakePathExt(gltf::GLB_EXTENSION)) {
        resourceWriter = std::make_unique<gltf::GLBResourceWriter>(std::move(streamWriter));
    }

    // The Document instance represents the glTF JSON manifest
    gltf::Document document;



    std::string accessorIdIndices;
    std::string accessorIdPositions;

    // Use the BufferBuilder helper class to simplify the process of
    // constructing valid glTF Buffer, BufferView and Accessor entities
    gltf::BufferBuilder bufferBuilder(std::move(resourceWriter));

    CreateTriangleResources(document, bufferBuilder, accessorIdIndices, accessorIdPositions);
    CreateTriangleEntities(document, accessorIdIndices, accessorIdPositions);

    std::string manifest;

    try {
        // Serialize the glTF Document into a JSON manifest
        manifest = Serialize(document, gltf::SerializeFlags::Pretty);
    }
    catch (const gltf::GLTFException& ex) {
        std::stringstream ss;

        ss << "Microsoft::glTF::Serialize failed: ";
        ss << ex.what();

        throw std::runtime_error(ss.str());
    }

    auto& gltfResourceWriter = bufferBuilder.GetResourceWriter();

    if (auto glbResourceWriter = dynamic_cast<gltf::GLBResourceWriter*>(&gltfResourceWriter)) {
        glbResourceWriter->Flush(manifest, pathFile.u8string()); // A GLB container isn't created until the GLBResourceWriter::Flush member function is called
    } else {
        gltfResourceWriter.WriteExternal(pathFile.u8string(), manifest); // Binary resources have already been written, just need to write the manifest
    }






    /*
    writeGltf(gltf_file_path, city_model, lod);

    // 中身が空ならOBJ削除
    if (v_offset_ == 0) {
        dll_logger_.lock()->log(DllLogLevel::LL_INFO, "No vertex generated. Deleting output obj.");
        fs::remove(gltf_file_path);
        return false;
    }

    if (options_.export_appearance) {
        // テクスチャファイルコピー
        for (const auto& [_, texture] : required_materials_) {
            const auto& texture_url = texture->getUrl();
            copyTexture(gml_file_path, gltf_file_path, texture_url);
        }

        writeMtl(gltf_file_path);
    }

    dll_logger_.lock()->log(DllLogLevel::LL_INFO, "Conversion succeeded");
    */
    return true;
}

void GltfWriter::writeGltf(const std::string& gltf_file_path, const citygml::CityModel& city_model, unsigned lod) {
    auto ofs = std::ofstream(gltf_file_path);
    if (!ofs.is_open()) {
        dll_logger_.lock()->throwException(std::string("Failed to open stream of obj path : ") + gltf_file_path);
    }
    ofs << std::fixed << std::setprecision(6);

    // MTL参照
    const auto mtl_file_name = fs::u8path(gltf_file_path).filename().replace_extension(".mtl").string();
    ofs << "mtllib " << mtl_file_name << std::endl;

    // エリア単位の場合拡張子無しファイル名がメッシュ名になります。
    if (options_.mesh_granularity == MeshGranularity::PerCityModelArea) {
        startMeshGroup(ofs, fs::u8path(gltf_file_path).filename().replace_extension().string());
    }

    for (const auto& root_object : city_model.getRootCityObjects()) {
        writeCityObjectRecursive(ofs, *root_object, lod);
    }
}

void GltfWriter::writeCityObjectRecursive(std::ofstream& ofs, const citygml::CityObject& target_object, unsigned lod) {
    writeCityObject(ofs, target_object, lod);

    for (unsigned i = 0; i < target_object.getChildCityObjectsCount(); i++) {
        const auto& child = target_object.getChildCityObject(i);
        writeCityObjectRecursive(ofs, child, lod);
    }
}

void GltfWriter::writeCityObject(std::ofstream& ofs, const citygml::CityObject& target_object, unsigned lod) {
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

void GltfWriter::writeGeometry(std::ofstream& ofs, const citygml::Geometry& target_geometry) {
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

void GltfWriter::writeVertices(std::ofstream& ofs, const std::vector<TVec3d>& vertices) {
    for (TVec3d vertex : vertices) {
        polar_to_plane_cartesian().convert(vertex);
        vertex = convertPosition(vertex, options_.reference_point, options_.mesh_axes, options_.unit_scale);
        ofs << generateVertex(vertex);
    }
}

void GltfWriter::writeUVs(std::ofstream& ofs, const std::vector<TVec2f>& uvs) {
    for (const auto& uv : uvs) {
        ofs << "vt " << uv.x << " " << uv.y << std::endl;
    }
}

void GltfWriter::writeIndices(std::ofstream& ofs, const std::vector<unsigned int>& indices) {
    unsigned face[3];
    for (unsigned i = 0; i < indices.size(); i++) {
        face[i % 3] = indices[i] + v_offset_ + 1;

        if (i % 3 < 2) {
            continue;
        }

        ofs << generateFace(face[0], face[1], face[2]);
    }
}

void GltfWriter::writeIndicesWithUV(std::ofstream& ofs, const std::vector<unsigned int>& indices) {
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

void GltfWriter::writeMaterialReference(std::ofstream& ofs, const std::shared_ptr<const Texture>& texture) {
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

void GltfWriter::writeMtl(const std::string& gltf_file_path) {
    const auto mtl_file_path = fs::u8path(gltf_file_path).replace_extension(".mtl").string();
    auto mtl_ofs = std::ofstream(mtl_file_path);
    if (!mtl_ofs.is_open()) {
        dll_logger_.lock()->throwException(std::string("Failed to open mtl file: ") + mtl_file_path);
    }

    mtl_ofs << generateDefaultMtl();
    for (auto& [material_name, texture] : required_materials_) {
        const auto& texture_url = texture->getUrl();
        mtl_ofs << generateMtl(material_name, texture_url);
    }
}
