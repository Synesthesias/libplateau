#include <filesystem>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/texture.h>

#include <plateau/io/mesh_converter.h>

#include <plateau/io/obj_writer.h>

namespace fs = std::filesystem;

void MeshConverter::convert(
    const std::string& destination_directory,
    const std::string& gml_file_path,
    std::vector<std::string>& converted_files,
    std::shared_ptr<const citygml::CityModel> city_model,
    std::shared_ptr<PlateauDllLogger> logger
) const {
    if (city_model == nullptr) {
        ParserParams params;
        params.optimize = true;
        params.tesselate = true;
        params.maxLOD = options_.max_lod;
        params.minLOD = options_.min_lod;
        city_model = citygml::load(gml_file_path, params);
    }

    const auto destination = fs::path(destination_directory).string();
    const auto gml_file_name = fs::path(gml_file_path).filename().string();
    const auto base_obj_name = fs::path(gml_file_name).replace_extension(".obj").string();

    for (unsigned lod = options_.min_lod; lod <= options_.max_lod; lod++) {
        const auto obj_file_path = fs::path(destination).append("LOD" + std::to_string(lod) + "_" + base_obj_name).make_preferred().string();
        const auto result = ObjWriter().write(obj_file_path, gml_file_path, *city_model, options_, lod, logger);
        if (result) {
            converted_files.push_back(obj_file_path);
        }
    }
}

void MeshConverter::convert(
    const std::string& destination_directory,
    const std::string& gml_file_path,
    std::shared_ptr<const citygml::CityModel> city_model,
    std::shared_ptr<PlateauDllLogger> logger
) const {
    std::vector<std::string> converted_files;
    convert(destination_directory, gml_file_path, converted_files, city_model, logger);
}

MeshConvertOptions MeshConverter::getOptions() const {
    return options_;
}

void MeshConverter::setOptions(const MeshConvertOptions& options) {
    options_ = options;
}
