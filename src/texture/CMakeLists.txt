if(IOS OR ANDROID)
    # モバイル向けには texture_packer.cpp のビルドが通らないのでダミーに置き換えます。
    target_sources(plateau PRIVATE
        "texture_packer_dummy.cpp"
    )
else()
    target_sources(plateau PRIVATE
        "texture_packer.cpp"
        "texture_image.cpp"
        "jpeg_texture_image.cpp"
        "png_texture_image.cpp"
        "tiff_texture_image.cpp"
    )
endif()
