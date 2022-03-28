add_compile_definitions(LIBPLATEAU_BUILD)

add_library(plateau SHARED "")
target_link_libraries(plateau PUBLIC citygml xerces-c)
target_include_directories(plateau PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}")

add_subdirectory(io)
