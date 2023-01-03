if(NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
  target_sources(plateau PRIVATE
    "node.cpp" 
    "mesh.cpp"
    "sub_mesh.cpp"
    "mesh_extractor.cpp"
    "model.cpp"
    "grid_merger.cpp"
    "polygon_mesh_utils.cpp"
    "mesh_merger.cpp"
  )
else()
  target_sources(plateau PRIVATE
    "node.cpp" 
    "mesh.cpp"
    "sub_mesh.cpp"
    "mesh_extractor_dummy.cpp"
    "model.cpp"
    "grid_merger.cpp"
    "polygon_mesh_utils.cpp"
    "mesh_merger.cpp"
  )
endif()
