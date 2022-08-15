#pragma once
#include <string>
#include <fstream>

#include <citygml/citygml.h>
#include <libplateau_api.h>
#include <plateau_dll_logger.h>
#include <plateau/io/mesh_convert_options.h>

#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/BufferBuilder.h>
#include <GLTFSDK/GLTFResourceWriter.h>
#include <GLTFSDK/GLBResourceWriter.h>
#include <GLTFSDK/IStreamWriter.h>
#include <GLTFSDK/Serialize.h>

class LIBPLATEAU_EXPORT GltfWriter {
public:
    GltfWriter() :
        v_offset_(0), image_id_num_(0), texture_id_num_(0), node_name_(""), mesh_(), scene_(), material_ids_(), default_material_id_("") {
    }

    bool write(
        const std::string& gltf_file_path,
        const std::string& gml_file_path,
        const citygml::CityModel& city_model,
        MeshConvertOptions options, unsigned lod,
        std::shared_ptr<PlateauDllLogger> logger = nullptr);

private:
    // Gltf書き出し
    void writeGltf(const std::string& gltf_file_path, const citygml::CityModel& city_model, unsigned lod, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder);
    void writeCityObjectRecursive(const citygml::CityObject& target_object, unsigned lod, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder);
    void writeCityObject(const citygml::CityObject& target_object, unsigned lod, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder);
    void writeGeometry(const citygml::Geometry& target_geometry, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder);
    std::string writeVertices(const std::vector<TVec3d>& vertices, Microsoft::glTF::BufferBuilder& bufferBuilder);
    std::string writeIndices(const std::vector<unsigned int>& indices, Microsoft::glTF::BufferBuilder& bufferBuilder);
    std::string writeUVs(const std::vector<TVec2f>& uvs, Microsoft::glTF::BufferBuilder& bufferBuilder);
    std::string writeMaterialReference(const std::shared_ptr<const Texture>& texture, Microsoft::glTF::Document& document);
    void writeNode(Microsoft::glTF::Document& document);

    MeshConvertOptions options_;

    std::map<std::string, std::shared_ptr<const Texture>> required_materials_;
    unsigned v_offset_;

    std::weak_ptr<PlateauDllLogger> dll_logger_;

    Microsoft::glTF::Mesh mesh_;
    Microsoft::glTF::Scene scene_;
    std::string node_name_;
    int image_id_num_, texture_id_num_;
    std::map<std::string, std::string> material_ids_;
    std::string default_material_id_;
};
