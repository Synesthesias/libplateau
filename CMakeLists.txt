cmake_minimum_required(VERSION 3.8)

# MSVC_RUNTIME_LIBRARYプロパティを有効化
# 参考：https://cmake.org/cmake/help/latest/policy/CMP0091.html
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

# MDd or MTでコンパイル
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:DebugDLL>")

enable_language(CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

project("libplateau")

set(LIBPLATEAU_BINARY_DIR ${libplateau_BINARY_DIR}/bin)

# dependencies
## Xerces-C
### libcitygmlのFindXercesを通すために全てキャッシュ化
set(BUILD_SHARED_LIBS false CACHE BOOL "" FORCE)
add_subdirectory("3rdparty/xerces-c")
set(XERCESC_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/xerces-c/src" "${libplateau_BINARY_DIR}/3rdparty/xerces-c/src" CACHE STRING "" FORCE)
set(XERCESC_LIBRARY xerces-c CACHE STRING "" FORCE)

## libcitygml
### TODO: GDAL Support
set(LIBCITYGML_USE_GDAL OFF CACHE BOOL "" FORCE)
### libcitygmlのFindXercesを通すために手動で設定
set(XERCESC_WAS_STATIC ON CACHE BOOL "" FORCE)

set(LIBCITYGML_DYNAMIC ON CACHE BOOL "" FORCE)
add_subdirectory("3rdparty/libcitygml")
set(LIBCITYGML_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libcitygml/sources/include")

# Library generation
add_subdirectory("src")


# Examples
add_subdirectory("examples/log_skipped_elements")
add_subdirectory("examples/export_obj")


# Wrappers
add_subdirectory("wrappers/c")

# Copy data for testing
file(COPY data/ DESTINATION "${CMAKE_BINARY_DIR}/data")
