add_compile_definitions(LIBPLATEAU_BUILD)

add_library(plateau OBJECT "")
target_link_libraries(plateau PUBLIC citygml xerces-c)
target_include_directories(plateau PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}")

set_target_properties(plateau PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})

add_subdirectory(io)