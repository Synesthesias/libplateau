add_compile_definitions(LIBPLATEAU_BUILD)


if(WIN32)
    add_library(plateau SHARED "")
else()
    add_library(plateau OBJECT "")
    add_library(plateau_static STATIC $<TARGET_OBJECTS:plateau>)
    add_library(plateau_shared SHARED $<TARGET_OBJECTS:plateau>)
    set_target_properties(plateau_static plateau_shared PROPERTIES OUTPUT_NAME plateau)
endif()

target_link_libraries(plateau PUBLIC citygml xerces-c)
target_include_directories(plateau PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}")

set_target_properties(plateau PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})

add_subdirectory(io)
add_subdirectory(c_wrapper)
target_link_libraries(plateau PUBLIC c_wrapper)