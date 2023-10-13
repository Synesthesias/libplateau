if(IOS OR ANDROID)
    # モバイル向けには texture_packer.cpp のビルドが通らないのでダミーに置き換えます。
    target_sources(plateau PRIVATE
        "texture_packer_dummy.cpp"
        "texture_image_base_dummy.cpp"
        "map_zoom_level_searcher.cpp"
    )
else()
    target_sources(plateau PRIVATE
        "texture_packer.cpp"
        "jpeg_texture_image.cpp"
        "png_texture_image.cpp"
        "tiff_texture_image.cpp"
        "texture_image_base.cpp"
        "texture_atlas_canvas.cpp"
        "atlas_container.cpp"
        "map_zoom_level_searcher.cpp"
    )
endif()
