

# 【概要】
# PLATEAUの街モデルに関する機能を提供するネイティブライブラリ、libplateau をビルドします。

# 【実行方法】
# このCMakeListsの実行にあたってcmakeコマンドにどのような引数を渡せば良いかを知るには、
# README.md を参照するか、 .github/actions/build/action.yml の Configure CMake の箇所を参照してコマンド例とします。

# 【ビルドの方針】
# [Unity向けビルドの場合]
# 最終的に libplateau.dll (または .so , .dylib ) が1つだけ出力されるようにします(example, libを除く)。
# なぜなら soファイルが複数できてしまうと、Unityに取り込んだときに .so同士の依存関係が解決されず DllNotFoundExceptionになるためです。
# ビルド時の依存関係を矢印で表すと次のようになります。
# plateau.dll (c_wrapperを含む)  --> (3rdparty/libcitygml, 3rdparty/xerces_c)
#
# [Unreal Engine向けビルドの場合]
# 静的ライブラリをビルドします。

# 【トラブルシューティング】
# Linux の Unity で DllNotFoundException になった場合
# .soファイルが存在しても、 .so が依存する .so ファイルが存在しない場合に DllNotFoundException になります。
# 次のコマンドで .so がどのようなライブラリに依存するか調べると良いです。
# objdump -x libplateau.so | grep NEEDED
# ldd ./libplateau.so | grep "not found”


cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0076 NEW)

# 引数チェック
# cmakeの引数で、次の変数が指定される必要があります。
# ・ -D BUILD_LIB_TYPE=static or dynamic        ビルドするのは静的ライブラリか動的ライブラリか
# ・ -D RUNTIME_LIB_TYPE=MT or MD               (Windowsのみ) MT か MD か
# デフォルト値は Unity向けに dynamic, MD です。
# 補足:
# 以前は Unity 向けは MT, Unreal向けは MD のつもりでしたが、MTビルドだとうまくいかないので今は全部 MD になってます。
# ここで MT と MD の切り替え機能があるのはその名残です。
SET(BUILD_LIB_TYPE "dynamic" CACHE STRING "static or dynamic")
SET(RUNTIME_LIB_TYPE "MD" CACHE STRING "MT or MD")
if(NOT ( BUILD_LIB_TYPE STREQUAL "static" OR BUILD_LIB_TYPE STREQUAL "dynamic"))
  message(FATAL_ERROR "Variable not set. use -D BUILD_LIB_TYPE=static or dynamic")
endif()
if(NOT (RUNTIME_LIB_TYPE STREQUAL "MT" OR RUNTIME_LIB_TYPE STREQUAL "MD"))
  message(FATAL_ERROR "Variable not set. use -D RUNTIME_LIB_TYPE=MT or MD")
endif()

# MSVC_RUNTIME_LIBRARYプロパティを有効化
# 参考：https://cmake.org/cmake/help/latest/policy/CMP0091.html
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

# (Windows向け) MT or MD or MTd or MDd のどれでコンパイルするか決めます。
# CMAKE_MSVC_RUNTIME_LIBRARY の値は最終的に次の4択のどれかになります:
# MultiThreaded, MultiThreadedDebug, MultiThreadedDll, MultiThreadedDebugDll
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  string(APPEND CMAKE_MSVC_RUNTIME_LIBRARY "Debug" )
endif()
if(RUNTIME_LIB_TYPE STREQUAL "MD")
  string(APPEND CMAKE_MSVC_RUNTIME_LIBRARY "DLL")
endif()
message("CMAKE_MSVC_RUNTIME_LIBRARY = ${CMAKE_MSVC_RUNTIME_LIBRARY}")



enable_language(CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
message(compiler_id = ${CMAKE_CXX_COMPILER_ID}, compiler_version = ${CMAKE_CXX_COMPILER_VERSION})

# Xerces-C ライブラリの問題を修正するための措置です。
# C言語で Windows かどうかチェックするには通常 #if define (_WIN32) と書きますが、
# Xerces-C の作者は _WIN32 のアンダースコアを書き忘れてしまいました。
# WIN32 と _WIN32 が同等の働きをするかは不明瞭であり、環境によってビルドが通ることも通らないこともあります。
# そこで WIN32 を定義することでビルドを通します。
if(WIN32) # CMake文法上の WIN32
  add_definitions(-D WIN32) # C言語の #define キーワードとしての WIN32
endif()

set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)

project("libplateau")

set(LIBPLATEAU_BINARY_DIR ${libplateau_BINARY_DIR}/bin)

# Linuxで shared library を作るときにはコンパイルオプション -fPIC が必要なので付加します。
# ただし、Windowsの MSVC でコンパイルするときは -fPIC が不要で、付けると警告が出るので付加しません。
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-fPIC COMPILER_SUPPORTS_PIC)
if(COMPILER_SUPPORTS_PIC AND (BUILD_LIB_TYPE STREQUAL "dynamic"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()


# ソースファイルの文字コードはデフォルトでは、 Windows日本語版のMSVC だと Shift_JIS、それ以外だと UTF-8 です。
# 今回は文字コードを統一したいので UTF-8 にします。
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if( NOT(IOS OR ANDROID) )
  ## ユニットテスト
  add_subdirectory("test")

  # dependencies
  ## Xerces-C をビルドします。
  ### libcitygmlのFindXercesを通すために全てキャッシュ化
  set(BUILD_SHARED_LIBS false CACHE BOOL "" FORCE)
  ### Xerces-cのうちネットワーク機能はビルドしません
  set(network false CACHE BOOL false FORCE)
  ### macOSの場合に Xerces-C が利用する transcodedr を指定します。
  if(APPLE)
    set(transcoder macosunicodeconverter CACHE BOOL macosunicodeconverter FORCE)
  endif()
  add_subdirectory("3rdparty/xerces-c")
  set(XERCESC_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/xerces-c/src" "${libplateau_BINARY_DIR}/3rdparty/xerces-c/src" CACHE STRING "" FORCE)
  set(XERCESC_LIBRARY xerces-c CACHE STRING "" FORCE)
endif()

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

if( NOT(IOS OR ANDROID) )
  # FBX SDK 以下のディレクトリで debug と release のどちらを利用するか決めます。
  # debug なら debug/*-md.lib を使い、release なら release/*-mt.lib を使います。
  if(RUNTIME_LIB_TYPE STREQUAL "MD")
    set(FBXSDK_MT_OR_MD "md")
  else()
    set(FBXSDK_MT_OR_MD "mt")
  endif()
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FBXSDK_LAST_DIR_NAME "debug")
  else()
    set(FBXSDK_LAST_DIR_NAME "release")
  endif()

  ## zlib(FBX SDKの依存関係)
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib")
  add_subdirectory("3rdparty/zlib")
  set(ZLIB_INCLUDE_DIR "${CMAKE_BINARY_DIR}/3rdparty/zlib")

  if(UNIX)
    set(ZLIB_LIBRARY "${CMAKE_BINARY_DIR}/3rdparty/zlib/libz.a" CACHE STRING "" FORCE)
  elseif(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(ZLIB_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fbx_sdk/2020.3.1/lib/win64/${FBXSDK_LAST_DIR_NAME}/zlib-${FBXSDK_MT_OR_MD}.lib" CACHE STRING "" FORCE)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(ZLIB_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fbx_sdk/2020.3.1/lib/win32/${FBXSDK_LAST_DIR_NAME}/zlib-${FBXSDK_MT_OR_MD}.lib" CACHE STRING "" FORCE)
    endif()
  endif(UNIX)

  ## libxml2(FBX SDKの依存関係)
  if(UNIX)
    # FBX SDKのLinux, macOS向けライブラリではiconvが必要
    set(LIBXML2_WITH_ICONV ON CACHE BOOL "" FORCE)
  elseif(WIN32)
    set(LIBXML2_WITH_ICONV OFF CACHE BOOL "" FORCE)
  endif()
  set(LIBXML2_WITH_LZMA OFF CACHE BOOL "" FORCE)
  set(LIBXML2_WITH_PYTHON OFF CACHE BOOL "" FORCE)
  set(LIBXML2_WITH_TESTS OFF CACHE BOOL "" FORCE)

  add_subdirectory("3rdparty/libxml2")

  ## FBX SDK
  set(FBXSDK_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fbx_sdk/2020.3.1/include")

  if(UNIX)
    if (APPLE)
      set(FBXSDK_LIB_PATH_SUFFIXES mac)
    else()
      if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(FBXSDK_LIB_PATH_SUFFIXES linux64)
      elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(FBXSDK_LIB_PATH_SUFFIXES linux32)
      endif()
    endif()
  elseif(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FBXSDK_LIB_PATH_SUFFIXES win64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(FBXSDK_LIB_PATH_SUFFIXES win32)
    endif()
  endif()

  set(FBXSDK_LIB_PATH_SUFFIXES "${FBXSDK_LIB_PATH_SUFFIXES}/${FBXSDK_LAST_DIR_NAME}")
  set(FBXSDK_LIB_NAME "libfbxsdk")
  if(WIN32)
    string(APPEND FBXSDK_LIB_NAME "-${FBXSDK_MT_OR_MD}")
  else()
    string(APPEND FBXSDK_LIB_NAME ".a")
  endif()

  find_library(FBXSDK_LIBRARIES
    NAMES
      ${FBXSDK_LIB_NAME}
    PATHS
      ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fbx_sdk/2020.3.1/lib
    PATH_SUFFIXES
      ${FBXSDK_LIB_PATH_SUFFIXES}
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
  )
  message("${FBXSDK_LIB_PATH_SUFFIXES} , ${FBXSDK_LIBRARIES}")
endif()

## openssl
add_subdirectory("3rdparty/openssl-cmake")

# cpp-httplib
set(HTTPLIB_COMPILE TRUE CACHE BOOL TRUE FORCE)
add_subdirectory("3rdparty/cpp-httplib")
set(CPPHTTPLIB_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cpp-httplib")

####################################
# libplateau 本体の Cmake ファイルです。

add_subdirectory("src")

####################################

if( NOT(IOS OR ANDROID) )
  ## Examples
  add_subdirectory("examples/log_skipped_elements")
  add_subdirectory("examples/export_obj")
  add_subdirectory("examples/export_gltf")
  add_subdirectory("examples/export_fbx")
  add_subdirectory("examples/https_mock_test")
endif()

# python
option(BUILD_PYTHON "Build python wrapper" OFF)
if(BUILD_PYTHON)
  add_subdirectory("wrappers/python")
endif(BUILD_PYTHON)

# Copy data for testing
file(COPY data/ DESTINATION "${CMAKE_BINARY_DIR}/data")

if(IOS)
  set_target_properties(ssl PROPERTIES
    XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
    XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
  )
  set_target_properties(crypto PROPERTIES
    XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
    XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
  )
endif()
