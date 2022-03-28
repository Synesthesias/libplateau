project(export_obj)

add_executable(export_obj "export_obj.cpp")

target_link_libraries(export_obj PRIVATE plateau)

set_target_properties(export_obj PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})
