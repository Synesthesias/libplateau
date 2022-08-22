project(export_gltf)

add_executable(export_gltf "export_gltf.cpp")

target_link_libraries(export_gltf PRIVATE plateau citygml)

include_directories("${GLTFSDK_INCLUDE}")

set_target_properties(export_gltf PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})
