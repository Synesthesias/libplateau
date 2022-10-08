

# 【概要】
# PLATEAUの街モデルに関する機能を提供するネイティブライブラリ、libplateau をビルドします。

# 【実行方法】
# このCMakeListsの実行にあたってcmakeコマンドにどのような引数を渡せば良いかを知るには、
# README.md を参照するか、 .github/actions/build/action.yml の Configure CMake の箇所を参照してコマンド例とします。

# 【ビルドの方針】
# 最終的に libplateau.dll (または .so , .dylib ) が1つだけ出力されるようにします(example, libを除く)。
# なぜなら soファイルが複数できてしまうと、Unityに取り込んだときに .so同士の依存関係が解決されず DllNotFoundExceptionになるためです。
# ビルド時の依存関係を矢印で表すと次のようになります。
# plateau.dll (c_wrapperを含む)  --> (3rdparty/libcitygml, 3rdparty/xerces_c)

# 【トラブルシューティング】
# Linux の Unity で DllNotFoundException になった場合
# .soファイルが存在しても、 .so が依存する .so ファイルが存在しない場合に DllNotFoundException になります。
# 次のコマンドで .so がどのようなライブラリに依存するか調べると良いです。
# objdump -x libplateau.so | grep NEEDED
# ldd ./libplateau.so | grep "not found”


# github actions で cmakeのオプション -S -B を使いたいのでこれを最小バージョンとします。
cmake_minimum_required(VERSION 3.13)

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

# ソースファイルの文字コードはデフォルトでは、 Windows日本語版のMSVC だと Shift_JIS、それ以外だと UTF-8 です。
# 今回は文字コードを統一したいので UTF-8 にします。
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

## ユニットテスト
add_subdirectory("test")

# dependencies
## Xerces-C をビルドします。
### libcitygmlのFindXercesを通すために全てキャッシュ化
set(BUILD_SHARED_LIBS false CACHE BOOL "" FORCE)
add_subdirectory("3rdparty/xerces-c")
set(XERCESC_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/xerces-c/src" "${libplateau_BINARY_DIR}/3rdparty/xerces-c/src" CACHE STRING "" FORCE)
set(XERCESC_LIBRARY xerces-c CACHE STRING "" FORCE)

## libcitygml をビルドします。
### TODO: GDAL Support
set(LIBCITYGML_USE_GDAL OFF CACHE BOOL "" FORCE)
### libcitygmlのFindXercesを通すために手動で設定
set(XERCESC_WAS_STATIC ON CACHE BOOL "" FORCE)

set(LIBCITYGML_DYNAMIC OFF CACHE BOOL "" FORCE)
add_subdirectory("3rdparty/libcitygml")
set(LIBCITYGML_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libcitygml/sources/include")

## glTF-SDK
add_subdirectory("3rdparty/glTF-SDK")
set(GLTFSDK_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glTF-SDK/glTF-SDK/GLTFSDK/Inc")

# cpp-httplib
add_subdirectory("3rdparty/cpp-httplib")
set(CPPHTTPLIB_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cpp-httplib")

# libplateau をビルドします。
add_subdirectory("src")
## Examples
add_subdirectory("examples/log_skipped_elements")
add_subdirectory("examples/export_obj")
add_subdirectory("examples/export_gltf")

# python
option(BUILD_PYTHON "Build python wrapper" OFF)
if(BUILD_PYTHON)
  add_subdirectory("wrappers/python")
endif(BUILD_PYTHON)

# Copy data for testing
file(COPY data/ DESTINATION "${CMAKE_BINARY_DIR}/data")
