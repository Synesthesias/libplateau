project(log_skipped_elements)

add_executable(log_skipped_elements "log_skipped_elements.cpp")

target_link_libraries(log_skipped_elements citygml)

include_directories("${LIBCITYGML_INCLUDE}")

set_target_properties(log_skipped_elements PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})
