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

#include <plateau/mesh_writer/gltf_writer.h>

#include <cassert>
#include <codecvt>
#include <fstream>

#include <GLTFSDK/GLBResourceWriter.h>
#include <GLTFSDK/IStreamWriter.h>
#include <GLTFSDK/Serialize.h>
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/BufferBuilder.h>


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

    void copyTexture(const std::string& gltf_path, const std::string& texture_url, plateau::meshWriter::GltfWriteOptions options) {
        auto src_path = fs::u8path(texture_url);
        auto  t_path = fs::u8path(texture_url).filename();
        auto dst_path = fs::u8path(gltf_path).parent_path();
        if (!options.texture_directory_path.empty()) {
            dst_path /= fs::u8path(options.texture_directory_path);
            create_directories(dst_path);
            dst_path /= t_path;
        } else {
            dst_path /= t_path;
        }

        constexpr auto copy_options = fs::copy_options::skip_existing;
        copy(src_path, dst_path, copy_options);
    }
}

namespace plateau::meshWriter {
    class GltfWriter::Impl {
    public:
        Impl() : image_id_num_(0), texture_id_num_(0), node_name_(""), scene_(), mesh_(),
            material_ids_(), current_material_id_(), default_material_id_(""), required_materials_(), options_() {
        }

        void precessNodeRecursive(const plateau::polygonMesh::Node& node, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder);
        std::string writeMaterialReference(std::string texUrl, Microsoft::glTF::Document& document);
        void writeNode(Microsoft::glTF::Document& document);
        void writeMesh(std::string accessorIdPositions, std::string accessorIdIndices, std::string accessorIdTexCoords, Microsoft::glTF::BufferBuilder& bufferBuilder);

        Microsoft::glTF::Scene scene_;
        Microsoft::glTF::Mesh mesh_;
        std::map<std::string, std::string> required_materials_;
        std::string node_name_;
        int image_id_num_, texture_id_num_;
        std::map<std::string, std::string> material_ids_;
        std::string default_material_id_, current_material_id_;
        GltfWriteOptions options_;
    };

    GltfWriter::GltfWriter() : impl(new Impl) {
    }

    GltfWriter::~GltfWriter() {
    }

    bool GltfWriter::write(const std::string& gltf_file_path_utf8, const plateau::polygonMesh::Model& model, GltfWriteOptions options) {

        impl->required_materials_.clear();
        impl->node_name_ = "";
        impl->image_id_num_ = 0;
        impl->texture_id_num_ = 0;
        impl->scene_.nodes.clear();
        impl->mesh_.primitives.clear();
        impl->material_ids_.clear();
        impl->options_ = options;

        std::filesystem::path path = std::filesystem::u8path(gltf_file_path_utf8);
        if (path.is_relative()) {
            auto pathCurrent = std::filesystem::current_path();
            pathCurrent /= path;
            pathCurrent.swap(path);
        }

        auto streamWriter = std::make_unique<StreamWriter>(path.parent_path());

        std::filesystem::path pathFile = path.filename();

        std::unique_ptr < gltf::ResourceWriter > resourceWriter;

        // create a GLTFResourceWriter
        if (options.mesh_file_format == plateau::meshWriter::GltfFileFormat::GLTF) {
            resourceWriter = std::make_unique<gltf::GLTFResourceWriter>(std::move(streamWriter));
        }

        // create a GLBResourceWriter. This class derives
        // from GLTFResourceWriter and adds support for writing manifests to a GLB container's
        // JSON chunk and resource data to the binary chunk.
        if (options.mesh_file_format == plateau::meshWriter::GltfFileFormat::GLB) {
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
        impl->default_material_id_ = document.materials.Append(material, gltf::AppendIdPolicy::GenerateOnEmpty).id;

        for (int i = 0; i < model.getRootNodeCount(); i++) {
            auto& root_node = model.getRootNodeAt(i);
            impl->precessNodeRecursive(root_node, document, bufferBuilder);
        }

        document.SetDefaultScene(std::move(impl->scene_), gltf::AppendIdPolicy::GenerateOnEmpty);
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

        // テクスチャファイルコピー
        for (const auto& [_, texture_url] : impl->required_materials_) {
            copyTexture(fs::absolute(path).u8string(), texture_url, options);
        }

        return true;
    }

    void GltfWriter::Impl::precessNodeRecursive(const plateau::polygonMesh::Node& node, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder) {

        node_name_ = node.getName();

        auto& mesh = node.getMesh();
        if (mesh.has_value()) {
            const auto& vertices = mesh.value().getVertices();
            const auto& all_indices = mesh.value().getIndices();
            const auto& uvs = mesh.value().getUV1();

            const auto& sub_meshes = mesh.value().getSubMeshes();
            if (!sub_meshes.empty()) {

                //position
                std::vector<float> positions;
                for (TVec3d vertex : vertices) {
                    positions.push_back((float)vertex.x);
                    positions.push_back((float)vertex.y);
                    positions.push_back((float)vertex.z);
                }
                bufferBuilder.AddBufferView(gltf::BufferViewTarget::ARRAY_BUFFER);
                std::vector<float> minValues(3U, std::numeric_limits<float>::max());
                std::vector<float> maxValues(3U, std::numeric_limits<float>::lowest());
                const size_t positionCount = positions.size();
                for (size_t i = 0U, j = 0U; i < positionCount; ++i, j = (i % 3U)) {
                    minValues[j] = std::min(positions[i], minValues[j]);
                    maxValues[j] = std::max(positions[i], maxValues[j]);
                }
                auto accessorIdPositions = bufferBuilder.AddAccessor(positions, { gltf::TYPE_VEC3, gltf::COMPONENT_FLOAT, false, minValues, maxValues }).id;

                //uv
                std::string accessorIdTexCoords = "";
                if (0 < uvs.size()) {
                    std::vector<float> texcoords;
                    for (const auto& uv : uvs) {
                        texcoords.push_back((float)uv.x);
                        texcoords.push_back((float)1.0 - (float)uv.y);
                    }
                    bufferBuilder.AddBufferView(gltf::BufferViewTarget::ARRAY_BUFFER);
                    accessorIdTexCoords = bufferBuilder.AddAccessor(texcoords, { gltf::TYPE_VEC2, gltf::COMPONENT_FLOAT }).id;
                }

                bufferBuilder.AddBufferView(gltf::BufferViewTarget::ELEMENT_ARRAY_BUFFER);
                for (auto& sub_mesh : sub_meshes) {
                    //index
                    auto st = sub_mesh.getStartIndex();
                    auto ed = sub_mesh.getEndIndex();
                    std::vector<int> indices(all_indices.begin() + st, all_indices.begin() + ed + 1);

                    auto& accessorIdIndices = bufferBuilder.AddAccessor(indices, { gltf::TYPE_SCALAR, gltf::COMPONENT_UNSIGNED_INT }).id;

                    //texture
                    auto& texUrl = sub_mesh.getTexturePath();
                    current_material_id_ = default_material_id_;
                    if (!texUrl.empty()) {
                        current_material_id_ = writeMaterialReference(texUrl, document);
                        writeMesh(accessorIdPositions, accessorIdIndices, accessorIdTexCoords, bufferBuilder);
                    } else {
                        writeMesh(accessorIdPositions, accessorIdIndices, "", bufferBuilder);
                    }
                }
                writeNode(document);
            }
        }

        for (size_t i = 0; i < node.getChildCount(); i++) {
            precessNodeRecursive(node.getChildAt(i), document, bufferBuilder);
        }
    }

    void GltfWriter::Impl::writeMesh(std::string accessorIdPositions, std::string accessorIdIndices, std::string accessorIdTexCoords, Microsoft::glTF::BufferBuilder& bufferBuilder) {
        gltf::MeshPrimitive meshPrimitive;
        meshPrimitive.materialId = current_material_id_;
        meshPrimitive.indicesAccessorId = accessorIdIndices;
        meshPrimitive.attributes[gltf::ACCESSOR_POSITION] = accessorIdPositions;
        if (accessorIdTexCoords != "") meshPrimitive.attributes[gltf::ACCESSOR_TEXCOORD_0] = accessorIdTexCoords;

        mesh_.primitives.push_back(meshPrimitive);
    }

    void GltfWriter::Impl::writeNode(gltf::Document& document) {
        auto meshId = document.meshes.Append(mesh_, gltf::AppendIdPolicy::GenerateOnEmpty).id;
        gltf::Node node;
        node.meshId = meshId;
        node.name = node_name_;
        auto nodeId = document.nodes.Append(node, gltf::AppendIdPolicy::GenerateOnEmpty).id;
        scene_.nodes.push_back(nodeId);
        mesh_.primitives.clear();
    }

    std::string GltfWriter::Impl::writeMaterialReference(std::string texture_url, gltf::Document& document) {
        // マテリアル名はテクスチャファイル名(拡張子抜き)
        const auto material_name = fs::u8path(texture_url).filename().replace_extension().u8string();
        const bool material_exists = required_materials_.find(material_name) != required_materials_.end();

        if (!material_exists) {
            Microsoft::glTF::Image image;

            required_materials_[material_name] = texture_url;

            image.id = std::to_string(image_id_num_);
            image_id_num_++;
            auto uri_str = fs::u8path(texture_url).filename().u8string();
            if (!options_.texture_directory_path.empty()) {
                uri_str = fs::u8path(options_.texture_directory_path).u8string() + "/" + uri_str;
            }
            image.uri = uri_str;
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
}
