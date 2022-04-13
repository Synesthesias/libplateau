enable_language(C)

project("libplateau_c")

add_library(plateau_c SHARED
  "obj_writer_c.cpp"
  "libplateau_c.h"
  "citygml_c.cpp"
  "city_model_c.h"
  "city_model_c.cpp"
  "city_object_c.cpp"
  "object_c.cpp"
  "featureobject_c.cpp")

set_target_properties(plateau_c PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})

target_link_libraries(plateau_c PRIVATE citygml plateau)
target_include_directories(plateau PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${LIBCITYGML_INCLUDE}")
