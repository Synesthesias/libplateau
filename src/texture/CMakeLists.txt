if(IOS OR ANDROID)
    # モバイル向けには texture_packer.cpp のビルドが通らないのでダミーに置き換えます。
    target_sources(plateau PRIVATE
        "texture_packer_dummy.cpp"
        "texture_image_base_dummy.cpp"
    )
else()
    # 非モバイル向け
    target_sources(plateau PRIVATE
        "texture_packer.cpp"
        "texture_image_base.cpp"
        "jpeg_texture_image.cpp"
        "png_texture_image.cpp"
        "tiff_texture_image.cpp"
        "texture_atlas_canvas.cpp"
        "atlas_container.cpp"
        "map_zoom_level_searcher.cpp"
    )
endif()

# モバイルと非モバイルで共通
target_sources(plateau PRIVATE
        "map_zoom_level_searcher.cpp"
)
