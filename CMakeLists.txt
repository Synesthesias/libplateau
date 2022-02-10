cmake_minimum_required(VERSION 3.8)

project("libplateau")


# dependencies
### Replace dynamic linking to static linking to build libcitygml samples.
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

## Xerces-C
set(BUILD_SHARED_LIBS false)
add_subdirectory("3rdparty/xerces-c")
set(XERCESC_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/xerces-c/src" "${libplateau_BINARY_DIR}/3rdparty/xerces-c/src")
set(XERCESC_LIBRARY xerces-c)

## libcitygml
### TODO: GDAL Support
set(LIBCITYGML_USE_GDAL OFF CACHE BOOL "" FORCE)
set(LIBCITYGML_DYNAMIC OFF CACHE BOOL "" FORCE)
add_subdirectory("3rdparty/libcitygml")
set(LIBCITYGML_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libcitygml/sources/include")


# Library generation
add_subdirectory("src")


# Examples
add_subdirectory("examples/log_skipped_elements")
add_subdirectory("examples/export_obj")


# Copy data for testing
file(COPY data/ DESTINATION "${CMAKE_BINARY_DIR}/data")