enable_language(C)

#project("libplateau_c")

add_library(c_wrapper OBJECT
  "libplateau_c.h"
  "citygml_c.cpp"
  "city_model_c.h"
  "city_model_c.cpp"
  "city_object_c.cpp"
  "object_c.cpp"
  "featureobject_c.cpp"
  "attributesmap_c.cpp"
  "attributevalue_c.cpp"
  "address_c.cpp"
  "geometry_c.cpp"
  "polygon_c.cpp"
  "linearring_c.cpp"
  "appearancetarget_c.cpp"
  "texturetargetdefinition_c.cpp"
  "texturecoordinates_c.cpp"
  "appearancetargetdefinition_c.cpp"
  "materialtargetdefinition_c.cpp"
  "texture_c.cpp"
  "appearance_c.cpp"
  "plateau_dll_logger_c.cpp"
  "mesh_extractor_c.cpp"
  "mesh_c.cpp"
  "sub_mesh_c.cpp"
  "model_c.cpp"
  "node_c.cpp"
  "geometry_utils_c.cpp"
  "geo_reference_c.cpp"
  "local_dataset_accessor_c.cpp"
  "mesh_code_c.cpp"
  "gml_file_c.cpp"
  "city_model_package_info_c.cpp"
  "mesh_extract_options_c.cpp"
  "gltf_writer_c.cpp" 
  "obj_writer_c.cpp"
  "mesh_merger_c.cpp"
  "vector_tile_downloader_c.cpp"
  "tile_projection_c.cpp"
  "lod_searcher_c.cpp"
  "dataset_source_c.cpp"
  "vector_c.cpp"
  "i_dataset_accessor_c.cpp"
  "dataset_metadata_group_c.cpp"
  "dataset_metadata_c.cpp"
  "server_dataset_accessor_c.cpp"
  "client_c.cpp"
  )

#target_link_libraries(c_wrapper PRIVATE citygml)
target_include_directories(c_wrapper PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}")

set_target_properties(c_wrapper PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})

include_directories("${GLTFSDK_INCLUDE}")
