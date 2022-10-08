cmake_minimum_required(VERSION 3.8)

project("libplateau-test")

if(MSVC)
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_DEBUG            ${CMAKE_C_FLAGS_DEBUG})
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_MINSIZEREL       ${CMAKE_C_FLAGS_MINSIZEREL})
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE          ${CMAKE_C_FLAGS_RELEASE})
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELWITHDEBINFO   ${CMAKE_C_FLAGS_RELWITHDEBINFO})
    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG          ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_MINSIZEREL     ${CMAKE_CXX_FLAGS_MINSIZEREL})
    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE        ${CMAKE_CXX_FLAGS_RELEASE})
    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
endif(MSVC)

enable_testing()

include(GoogleTest)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/gtest")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/gtest/googletest/include")

add_executable(plateau_test
    "test_mesh_converter.cpp"
    "test_primary_city_object_types.cpp"
    "test_mesh_extractor.cpp"
    "test_grid_merger.cpp"
    "test_mesh_code.cpp"
    "test_udx.cpp"
    "test_vector_tile.cpp")

target_link_libraries(plateau_test gtest gtest_main plateau citygml)

set_target_properties(plateau_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})
