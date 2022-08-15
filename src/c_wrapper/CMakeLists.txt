enable_language(C)

#project("libplateau_c")

add_library(c_wrapper OBJECT
  "mesh_converter_c.cpp"
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
  "mesh_merger_c.cpp")

#target_link_libraries(c_wrapper PRIVATE citygml)
target_include_directories(c_wrapper PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}")

set_target_properties(c_wrapper PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})
