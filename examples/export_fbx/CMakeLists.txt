project(export_fbx)

add_executable(export_fbx "export_fbx.cpp")

target_link_libraries(export_fbx PRIVATE plateau citygml "${FBXSDK_LIBRARIES}")

include_directories("${GLTFSDK_INCLUDE}")

set_target_properties(export_fbx PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})
