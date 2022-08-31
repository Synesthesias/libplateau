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

#include <plateau/geometry/primary_city_object_types.h>

#include "gltf_writer.h"
#include "../io/polar_to_plane_cartesian.h"

#include <cassert>

namespace fs = std::filesystem;
namespace gltf = Microsoft::glTF;
using namespace citygml;

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
    v_offset_total_ = 0;
    required_materials_.clear();
    node_name_ = "";
    image_id_num_ = 0;
    texture_id_num_ = 0;
    scene_.nodes.clear();
    mesh_.primitives.clear();
    material_ids_.clear();
    positions_.clear();
    texcoords_.clear();
    indices_.clear();
    current_material_id_ = "";

    std::filesystem::path path = gltf_file_path;
    if (path.is_relative()) {
        auto pathCurrent = std::filesystem::current_path();
        pathCurrent /= path;
        pathCurrent.swap(path);
    }

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

    gltf::Document document;

    gltf::BufferBuilder bufferBuilder(std::move(resourceWriter));

    const char* bufferId = nullptr;
    std::string bi = pathFile.filename().replace_extension().string();
    
    if (dynamic_cast<const gltf::GLBResourceWriter*>(&bufferBuilder.GetResourceWriter())) {
        bufferId = gltf::GLB_BUFFER_ID;
    } else {
        bufferId = bi.c_str();
    }

    bufferBuilder.AddBuffer(bufferId);

    gltf::Material material;
    material.metallicRoughness.baseColorFactor = gltf::Color4(0.5f, 0.5f, 0.5f, 1.0f);
    material.metallicRoughness.metallicFactor = 0.0f;
    material.metallicRoughness.roughnessFactor = 1.0f;
    default_material_id_ = document.materials.Append(material, gltf::AppendIdPolicy::GenerateOnEmpty).id;

    writeGltf(gltf_file_path, city_model, lod, document, bufferBuilder);

    // for last node
    if (!indices_.empty()) {
        writeMesh(document, bufferBuilder);
        writeNode(document);
    }

    document.SetDefaultScene(std::move(scene_), gltf::AppendIdPolicy::GenerateOnEmpty);
    bufferBuilder.Output(document);
    std::string manifest;

    try {
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

    if (options_.export_appearance) {
        // テクスチャファイルコピー
        for (const auto& [_, texture] : required_materials_) {
            const auto& texture_url = texture->getUrl();
            copyTexture(gml_file_path, gltf_file_path, texture_url);
        }
    }
    
    // 中身が空ならファイル削除
    if (v_offset_total_ == 0) {
        dll_logger_.lock()->log(DllLogLevel::LL_INFO, "No vertex generated. Deleting output obj.");
        fs::remove(gltf_file_path);
        return false;
    }

    dll_logger_.lock()->log(DllLogLevel::LL_INFO, "Conversion succeeded");
    
    return true;
}

void GltfWriter::writeGltf(const std::string& gltf_file_path, const citygml::CityModel& city_model, unsigned lod, gltf::Document& document, gltf::BufferBuilder& bufferBuilder) {
    
    // エリア単位の場合拡張子無しファイル名がメッシュ名になります。
    if (options_.mesh_granularity == MeshGranularity::PerCityModelArea) {
        node_name_ = fs::u8path(gltf_file_path).filename().replace_extension().string();
    }

    for (const auto& root_object : city_model.getRootCityObjects()) {
        writeCityObjectRecursive(*root_object, lod, document, bufferBuilder);
    }
}

void GltfWriter::writeCityObjectRecursive(const citygml::CityObject& target_object, unsigned lod, gltf::Document& document, gltf::BufferBuilder& bufferBuilder) {
    writeCityObject(target_object, lod, document, bufferBuilder);

    for (unsigned i = 0; i < target_object.getChildCityObjectsCount(); i++) {
        const auto& child = target_object.getChildCityObject(i);
        writeCityObjectRecursive(child, lod, document, bufferBuilder);
    }
}

void GltfWriter::writeCityObject(const citygml::CityObject& target_object, unsigned lod, gltf::Document& document, gltf::BufferBuilder& bufferBuilder) {
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
        if (!indices_.empty()) {
            writeMesh(document, bufferBuilder);
        }
        if (!mesh_.primitives.empty()) writeNode(document);
        
        std::stringstream ss;
        ss << "LOD" << lod << "_" << target_object.getId();
        node_name_ = ss.str();
    }

    for (unsigned i = 0; i < target_object.getGeometriesCount(); i++) {
        const auto target_lod = target_object.getGeometry(i).getLOD();
        if (target_lod == lod)
            writeGeometry(target_object.getGeometry(i), document, bufferBuilder);
    }
}

void GltfWriter::writeGeometry(const citygml::Geometry& target_geometry, gltf::Document& document, gltf::BufferBuilder& bufferBuilder) {
    for (unsigned int i = 0; i < target_geometry.getPolygonsCount(); i++) {
        const auto polygon = target_geometry.getPolygon(i);
        const auto& indices = polygon->getIndices();
        if (indices.empty()) continue;
        
        const auto uvs = polygon->getTexCoordsForTheme("rgbTexture", true);
        std::string materialId = "";
        if (!uvs.empty()) {          
            const auto texture = polygon->getTextureFor("rgbTexture");
            materialId = writeMaterialReference(texture, document);
        } else {
            materialId = default_material_id_;
        }
        if (current_material_id_ == "") current_material_id_ = materialId;

        if (current_material_id_ != materialId && !indices_.empty() ) {
            writeMesh(document, bufferBuilder);
            current_material_id_ = materialId;
        }

        if (!uvs.empty()) writeUVs(uvs, bufferBuilder);

        writeIndices(indices, bufferBuilder);

        const auto& vertices = polygon->getVertices();
        writeVertices(vertices, bufferBuilder);

        v_offset_ += vertices.size();
    }

    for (unsigned int i = 0; i < target_geometry.getGeometriesCount(); i++) {
        const auto& child_geometry = target_geometry.getGeometry(i);
        writeGeometry(child_geometry, document, bufferBuilder);
    }
}

void GltfWriter::writeVertices(const std::vector<TVec3d>& vertices, gltf::BufferBuilder& bufferBuilder) {
    for (TVec3d vertex : vertices) {
        polar_to_plane_cartesian().convert(vertex);
        vertex = convertPosition(vertex, options_.reference_point, options_.mesh_axes, options_.unit_scale);
        positions_.push_back((float)vertex.x);
        positions_.push_back((float)vertex.y);
        positions_.push_back((float)vertex.z);
    }
}

void GltfWriter::writeUVs(const std::vector<TVec2f>& uvs, gltf::BufferBuilder& bufferBuilder) {
    for (const auto& uv : uvs) {
        texcoords_.push_back((float)uv.x);
        texcoords_.push_back((float)1.0-(float)uv.y);
    }
}

void GltfWriter::writeIndices(const std::vector<unsigned int>& indices, gltf::BufferBuilder& bufferBuilder) {
    for (const auto& index : indices) {
        indices_.push_back(index + v_offset_);
    }
}

void GltfWriter::writeMesh(Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder) {
    // for index
    bufferBuilder.AddBufferView(gltf::BufferViewTarget::ELEMENT_ARRAY_BUFFER);
    auto accessorIdIndices = bufferBuilder.AddAccessor(indices_, { gltf::TYPE_SCALAR, gltf::COMPONENT_UNSIGNED_INT }).id;
    indices_.clear();
    v_offset_total_ += v_offset_;
    v_offset_ = 0;

    // for position
    bufferBuilder.AddBufferView(gltf::BufferViewTarget::ARRAY_BUFFER);
    std::vector<float> minValues(3U, std::numeric_limits<float>::max());
    std::vector<float> maxValues(3U, std::numeric_limits<float>::lowest());
    const size_t positionCount = positions_.size();
    for (size_t i = 0U, j = 0U; i < positionCount; ++i, j = (i % 3U)) {
        minValues[j] = std::min(positions_[i], minValues[j]);
        maxValues[j] = std::max(positions_[i], maxValues[j]);
    }
    auto accessorIdPositions = bufferBuilder.AddAccessor(positions_, { gltf::TYPE_VEC3, gltf::COMPONENT_FLOAT, false, minValues, maxValues }).id;
    positions_.clear();

    // for uvs
    std::string accessorIdTexCoords = "";
    if (!texcoords_.empty()) {
        bufferBuilder.AddBufferView(gltf::BufferViewTarget::ARRAY_BUFFER);
        accessorIdTexCoords = bufferBuilder.AddAccessor(texcoords_, { gltf::TYPE_VEC2, gltf::COMPONENT_FLOAT }).id;
    }
    texcoords_.clear();

    gltf::MeshPrimitive meshPrimitive;
    meshPrimitive.materialId = current_material_id_;
    meshPrimitive.indicesAccessorId = accessorIdIndices;
    meshPrimitive.attributes[gltf::ACCESSOR_POSITION] = accessorIdPositions;
    if (accessorIdTexCoords != "") meshPrimitive.attributes[gltf::ACCESSOR_TEXCOORD_0] = accessorIdTexCoords;

    mesh_.primitives.push_back(meshPrimitive);
    current_material_id_ = "";
}

void GltfWriter::writeNode(gltf::Document& document) {
    auto meshId = document.meshes.Append(mesh_, gltf::AppendIdPolicy::GenerateOnEmpty).id;
    gltf::Node node;
    node.meshId = meshId;
    node.name = node_name_;
    auto nodeId = document.nodes.Append(node, gltf::AppendIdPolicy::GenerateOnEmpty).id;
    scene_.nodes.push_back(nodeId);
    mesh_.primitives.clear();
}

std::string GltfWriter::writeMaterialReference(const std::shared_ptr<const Texture>& texture, gltf::Document& document) {
    // マテリアル名はテクスチャファイル名(拡張子抜き)
    const auto& texture_url = texture->getUrl();
    const auto material_name = fs::u8path(texture_url).filename().replace_extension().u8string();
    const bool material_exists = required_materials_.find(material_name) != required_materials_.end();

    if (!material_exists) {
        Microsoft::glTF::Image image;

        required_materials_[material_name] = texture;

        image.id = std::to_string(image_id_num_);
        image_id_num_++;
        image.uri = fs::u8path(texture_url).u8string();
        auto imageId = document.images.Append(image, gltf::AppendIdPolicy::GenerateOnEmpty).id;

        gltf::Texture tex;
        tex.imageId = imageId;
        tex.id = std::to_string(texture_id_num_);
        texture_id_num_++;
        auto textureId = document.textures.Append(tex, gltf::AppendIdPolicy::GenerateOnEmpty).id;

        gltf::Material material;
        material.metallicRoughness.baseColorFactor = gltf::Color4(0.5f, 0.5f, 0.5f, 1.0f);
        material.metallicRoughness.metallicFactor = 0.0f;
        material.metallicRoughness.roughnessFactor = 1.0f;
        material.metallicRoughness.baseColorTexture.textureId = textureId;
        auto materialId = document.materials.Append(material, gltf::AppendIdPolicy::GenerateOnEmpty).id;
        material_ids_[material_name] = materialId;
    }
    return material_ids_[material_name];
}
