if( IOS OR ANDROID)
  target_sources(plateau PRIVATE
      "obj_writer.cpp"
      "gltf_writer.cpp"
      "fbx_writer_dummy.cpp"
    )
else()
  target_sources(plateau PRIVATE
      "obj_writer.cpp"
      "gltf_writer.cpp"
      "fbx_writer.cpp"
    )
endif()
