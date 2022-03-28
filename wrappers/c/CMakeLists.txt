enable_language(C)

project("libplateau_c")

add_library(plateau_c SHARED "obj_writer_c.cpp" "libplateau_c.h")

target_link_libraries(plateau_c PRIVATE citygml plateau)
target_include_directories(plateau PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${LIBCITYGML_INCLUDE}")
