cmake_minimum_required(VERSION 3.8)

project("libplateau-test")

if(MSVC AND (RUNTIME_LIB_TYPE STREQUAL "MT"))
    set(NEW_COMPILE_OPTION "/MT")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        string(APPEND NEW_COMPILE_OPTION "d")
    endif()
    string(REPLACE "/MD" ${NEW_COMPILE_OPTION} CMAKE_C_FLAGS_DEBUG            ${CMAKE_C_FLAGS_DEBUG})
    string(REPLACE "/MD" ${NEW_COMPILE_OPTION} CMAKE_C_FLAGS_MINSIZEREL       ${CMAKE_C_FLAGS_MINSIZEREL})
    string(REPLACE "/MD" ${NEW_COMPILE_OPTION} CMAKE_C_FLAGS_RELEASE          ${CMAKE_C_FLAGS_RELEASE})
    string(REPLACE "/MD" ${NEW_COMPILE_OPTION} CMAKE_C_FLAGS_RELWITHDEBINFO   ${CMAKE_C_FLAGS_RELWITHDEBINFO})
    string(REPLACE "/MD" ${NEW_COMPILE_OPTION} CMAKE_CXX_FLAGS_DEBUG          ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/MD" ${NEW_COMPILE_OPTION} CMAKE_CXX_FLAGS_MINSIZEREL     ${CMAKE_CXX_FLAGS_MINSIZEREL})
    string(REPLACE "/MD" ${NEW_COMPILE_OPTION} CMAKE_CXX_FLAGS_RELEASE        ${CMAKE_CXX_FLAGS_RELEASE})
    string(REPLACE "/MD" ${NEW_COMPILE_OPTION} CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    message(NEW_COMPILE_OPTION = ${NEW_COMPILE_OPTION})
endif()

enable_testing()

include(GoogleTest)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gtest")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/gtest/googletest/include")

add_executable(plateau_test
    "test_primary_city_object_types.cpp"
    "test_mesh_extractor.cpp"
    "test_grid_merger.cpp"
    "test_mesh_code.cpp"
    "test_dataset.cpp"
    "test_vector_tile.cpp"
    "test_obj_writer.cpp"
    "test_gltf_writer.cpp"
    "test_mesh_merger.cpp"
    "test_model.cpp"
    "test_gml_file.cpp"
    "test_network_client.cpp"
    "test_dataset.cpp"
    "test_dataset_source.cpp"
    "test_server_dataset_accessor.cpp"
    "test_fbx_writer.cpp"
    "test_lod_searcher.cpp"
    "test_texture_packer.cpp"
    "test_map_attacher.cpp"
        )

target_link_libraries(plateau_test gtest gtest_main plateau citygml)

# Visual Studio 以外向けに出力先を設定します。
set_target_properties(plateau_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})

# Visual Studio 向けに出力先を設定します。
string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_TO_UPPER)
set_target_properties( plateau_test PROPERTIES "RUNTIME_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE_TO_UPPER}" "${LIBPLATEAU_BINARY_DIR}" )
