include(ExternalProject)

# Set compiler flags
if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_WARNINGS")
endif()

# Set WITH_SIMD
include(CheckLanguage)
check_language(ASM_NASM)
if (CMAKE_ASM_NASM_COMPILER)
    if (APPLE)
        # macOS might have /usr/bin/nasm but it cannot be used
        # https://stackoverflow.com/q/53974320
        # To fix this, run `brew install nasm`
        execute_process(COMMAND nasm --version RESULT_VARIABLE return_code)
        if("${return_code}" STREQUAL "0")
            enable_language(ASM_NASM)
            option(WITH_SIMD "" ON)
        else()
            message(STATUS "nasm found but can't be used, run `brew install nasm`")
            option(WITH_SIMD "" OFF)
        endif()
    else()
        enable_language(ASM_NASM)
        option(WITH_SIMD "" ON)
    endif()
else()
    option(WITH_SIMD "" OFF)
endif()
if (WITH_SIMD)
    message(STATUS "NASM assembler enabled")
else()
    message(STATUS "NASM assembler not found - libjpeg-turbo performance may suffer")
endif()

if (STATIC_WINDOWS_RUNTIME)
    set(WITH_CRT_DLL OFF)
else()
    set(WITH_CRT_DLL ON)
endif()
message(STATUS "libturbojpeg: WITH_CRT_DLL=${WITH_CRT_DLL}")

# If MSVC, the OUTPUT_NAME was set to turbojpeg-static
if(MSVC)
    set(lib_name "turbojpeg-static")
else()
    set(lib_name "turbojpeg")
endif()

ExternalProject_Add(
    libjpeg-turbo
    URL https://github.com/libjpeg-turbo/libjpeg-turbo/archive/refs/tags/3.0.0.tar.gz
    URL_HASH SHA256=171dae5d73560bc94006a7c0c3281bd9bfde6a34f7e41e66f930a1a9162bd7df
    DOWNLOAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libjpeg-turbo"
    INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/3rdparty/libjpeg-turbo"
    UPDATE_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DWITH_CRT_DLL=${WITH_CRT_DLL}
        -DENABLE_STATIC=ON
        -DENABLE_SHARED=OFF
        -DWITH_SIMD=${WITH_SIMD}
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/3rdparty/libjpeg-turbo
        ${ExternalProject_CMAKE_ARGS_hidden}
    BUILD_BYPRODUCTS
        <INSTALL_DIR>/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${lib_name}${CMAKE_STATIC_LIBRARY_SUFFIX}
)

ExternalProject_Get_Property(libjpeg-turbo INSTALL_DIR)
set(JPEG_TURBO_INCLUDE_DIRS "${INSTALL_DIR}/include/") # "/" is critical.
set(JPEG_TURBO_LIB_DIR "${INSTALL_DIR}/lib/")
set(JPEG_TURBO_LIBRARIES "${JPEG_TURBO_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib_name}${CMAKE_STATIC_LIBRARY_SUFFIX}")
