project(texture_packing)

add_executable(texture_packing "texture_packing.cpp")

target_link_libraries(texture_packing PRIVATE plateau citygml)

set_target_properties(texture_packing PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})
