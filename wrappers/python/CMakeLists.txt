cmake_minimum_required(VERSION 3.2)
project(python_wrapper)

set(PYTHON_EXECUTABLE D:/anaconda3/envs/cmake_test/python.exe)
#set(PYTHON_EXECUTABLE  "C:/Program Files/Blender Foundation/Blender 2.93/2.93/python/bin/python.exe")

add_subdirectory(pybind11)

pybind11_add_module(python_wrapper python_wrapper.cpp)

target_link_libraries(python_wrapper PRIVATE plateau)
target_include_directories(plateau PRIVATE "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}")
set_target_properties(python_wrapper PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIBPLATEAU_BINARY_DIR})

target_include_directories(python_wrapper PRIVATE ${PROJECT_SOURCE_DIR}/python3.9/Include)