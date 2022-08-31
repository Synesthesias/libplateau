add_compile_definitions(LIBPLATEAU_BUILD)


add_library(plateau SHARED "")

target_link_libraries(plateau PUBLIC citygml xerces-c GLTFSDK)
target_include_directories(plateau PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}" "${GLTFSDK_INCLUDE}")

set_target_properties(plateau PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})

add_subdirectory(io)
add_subdirectory(geometry)
add_subdirectory(c_wrapper)
target_link_libraries(plateau PUBLIC c_wrapper)
