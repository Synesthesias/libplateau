add_compile_definitions(LIBPLATEAU_BUILD)

add_library(plateau OBJECT "")
target_include_directories(plateau PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}")

set_target_properties(plateau PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})

add_subdirectory(io)

# TODO: citygmlが更新されてもplateau側が更新されていないとdllが反映されない問題
#if(WIN32)
#  if(CMAKE_BUILD_TYPE MATCHES Debug)
#    set(LIBCITYGML_DLL_PATH "${libcitygml_BINARY_DIR}/lib/citygmld.dll")
#  else()
#    set(LIBCITYGML_DLL_PATH "${libcitygml_BINARY_DIR}/lib/citygml.dll")
#  endif()
#  add_custom_command(
#    TARGET plateau POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy ${LIBCITYGML_DLL_PATH} ${LIBPLATEAU_BINARY_DIR})
#endif(WIN32)
