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


# Xerces-C ライブラリの問題を修正するための措置です。
# C言語で Windows かどうかチェックするには通常 #if define (_WIN32) と書きますが、
# Xerces-C の作者は _WIN32 のアンダースコアを書き忘れてしまいました。
# WIN32 と _WIN32 が同等の働きをするかは不明瞭であり、環境によってビルドが通ることも通らないこともあります。
# そこで WIN32 を定義することでビルドを通します。
if(WIN32) # CMake文法上の WIN32
  add_definitions(-D WIN32) # C言語の #define キーワードとしての WIN32
endif()

project("libplateau")

set(LIBPLATEAU_BINARY_DIR ${libplateau_BINARY_DIR}/bin)

# Linuxで shared library を作るときにはコンパイルオプション -fPIC が必要なので付加します。
# ただし、Windowsの MSVC でコンパイルするときは -fPIC が不要で、付けると警告が出るので付加しません。
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-fPIC COMPILER_SUPPORTS_PIC)
if(COMPILER_SUPPORTS_PIC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

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
