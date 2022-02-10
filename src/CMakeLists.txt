add_library(libplateau "")
target_link_libraries(libplateau PUBLIC citygml xerces-c)
target_include_directories(libplateau PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}")

add_subdirectory(io)
