add_compile_definitions(LIBPLATEAU_BUILD)

add_library(plateau SHARED "")
if( (NOT CMAKE_SYSTEM_NAME STREQUAL "iOS") AND (NOT ANDROID) )
  target_link_libraries(plateau PUBLIC citygml xerces-c GLTFSDK crypto ssl "${FBXSDK_LIBRARIES}" LibXml2 "${ZLIB_LIBRARY}" c_wrapper)
  target_include_directories(plateau PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}" "${GLTFSDK_INCLUDE}" "${CPPHTTPLIB_INCLUDE}" "${FBXSDK_INCLUDE}")
else()
  target_link_libraries(plateau PUBLIC 
    "citygml" 
    "GLTFSDK" 
    "crypto" 
    "ssl" 
    "c_wrapper"
  )
  set_target_properties(plateau PROPERTIES 
    FRAMEWORK TRUE
    MACOSX_FRAMEWORK_IDENTIFIER jp.synesthesias.libplateau
  )
  target_include_directories(plateau PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}" "${GLTFSDK_INCLUDE}" "${CPPHTTPLIB_INCLUDE}")
endif()

set_target_properties(plateau PROPERTIES RUNTIME_OUTPUT_DIRECTORY
  ${LIBPLATEAU_BINARY_DIR})

add_subdirectory(io)
add_subdirectory(mesh_writer)
add_subdirectory(polygon_mesh)
add_subdirectory(geometry)
add_subdirectory(basemap)
add_subdirectory(c_wrapper)
add_subdirectory(dataset)
add_subdirectory(network)
