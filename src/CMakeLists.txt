add_compile_definitions(LIBPLATEAU_BUILD)

if(BUILD_LIB_TYPE STREQUAL "dynamic")
  add_library(plateau SHARED "")
  target_compile_definitions(plateau PUBLIC LIBPLATEAU_BUILD_DYNAMIC)
  message("plateau is dynamic library.")
else()
  add_library(plateau STATIC ""
          ../include/plateau/granularity_convert/convert_from_atomic_to_material_in_primary.h
          granularity_convert/convert_from_atomic_to_material_in_primary.cpp)
endif()

if( NOT(IOS OR ANDROID) )
  target_link_libraries(plateau PUBLIC
          citygml xerces-c GLTFSDK crypto ssl "${FBXSDK_LIBRARIES}" LibXml2 "${ZLIB_LIBRARY}" c_wrapper OpenMeshCore OpenMeshTools
          )
  target_include_directories(plateau PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}" "${GLTFSDK_INCLUDE}" "${CPPHTTPLIB_INCLUDE}" "${FBXSDK_INCLUDE}")

  # 注意点:
  # libpngのバージョンは、Unreal EngineのPngImageWrapper.hに記載のバージョンに合わせる必要があります。
  # そうでないと、UEでパッケージ化したあとexeを起動したときにassertion failedのダイアログが出て強制終了になります。
  # Unreal Engineの対応バージョンを上げるときには併せてPngImageWrapperで利用のlibpngバージョンを確認し、
  # libpngのバージョンが上がる場合はlibplateau submoduleのlibpngを新しいバージョンにしたうえで下のpng15_staticの15を{major_ver}{minor_ver}に置き換える必要がありそうです。
  add_dependencies(plateau libjpeg-turbo png15_static tiff)
  target_link_libraries(plateau PRIVATE ${JPEG_TURBO_LIBRARIES} png15_static tiff)
  target_include_directories(plateau PRIVATE
    "${JPEG_TURBO_INCLUDE_DIRS}"
    "${PNG_INCLUDE_DIRS}"
    "${TIFF_INCLUDE}")
else()
  target_link_libraries(plateau PUBLIC 
    "citygml" 
    "GLTFSDK" 
    "crypto" 
    "ssl" 
    "c_wrapper"
    "png15_static"
    "OpenMeshCore"
    "OpenMeshTools"
  )

  # UnityでiOS向けのビルドを通すため、info.plistというファイルにバイナリの情報が入るようにします。
  # バージョンもUnityビルドに必要ですが、値は何でも良いのでずっと"1.0"のままで良さそうです。
  set_target_properties(plateau PROPERTIES 
    FRAMEWORK TRUE
    MACOSX_FRAMEWORK_IDENTIFIER jp.synesthesias.libplateau
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING "1.0"
    MACOSX_FRAMEWORK_BUNDLE_VERSION "1.0"
  )

  target_include_directories(plateau PUBLIC "${CMAKE_SOURCE_DIR}/include" "${LIBCITYGML_INCLUDE}" "${GLTFSDK_INCLUDE}" "${CPPHTTPLIB_INCLUDE}")
  target_include_directories(plateau PUBLIC "${PNG_INCLUDE_DIRS}")
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
add_subdirectory(texture)
add_subdirectory(granularity_convert)
add_subdirectory(height_map_aligner)
add_subdirectory(height_map_generator)
add_subdirectory(material_adjust)

# libファイルを1つに結合します。
if(BUILD_LIB_TYPE STREQUAL "static")

  set(COMBINE_LIB_FILES
          $<TARGET_FILE:plateau>
          $<TARGET_FILE:citygml>
          $<TARGET_FILE:xerces-c>
          $<TARGET_FILE:GLTFSDK>
          $<TARGET_FILE:crypto>
          $<TARGET_FILE:ssl>
          $<TARGET_FILE:LibXml2>
          $<TARGET_FILE:httplib>
          $<TARGET_FILE:zlibstatic>
          $<TARGET_FILE:png15_static>
          $<TARGET_FILE:tiff>
          # OpenMeshについては、普通に$<TARGET_FILE>すると、Windowsでは良いですがMacだと静的ライブラリが欲しいのに動的ライブラリが取得されてしまいます。
          # そのため、TARGET_FILEを使わない少々周りくどい方法でパスを取得します。
          $<TARGET_FILE_DIR:OpenMeshCore>/$<TARGET_FILE_PREFIX:OpenMeshCore>$<TARGET_FILE_BASE_NAME:OpenMeshCore>${CMAKE_STATIC_LIBRARY_SUFFIX}
          $<TARGET_FILE_DIR:OpenMeshTools>/$<TARGET_FILE_PREFIX:OpenMeshTools>$<TARGET_FILE_BASE_NAME:OpenMeshTools>${CMAKE_STATIC_LIBRARY_SUFFIX}
          ${JPEG_TURBO_LIBRARIES}
          ${FBXSDK_LIBRARIES})

  # windows で libファイル結合します。
  # 参考 : https://stackoverflow.com/questions/60190374/how-to-bundle-multiple-static-libraries-into-single-library-in-cmake-for-windows
  if(BUILD_LIB_TYPE STREQUAL "static")
    set(COMBINE_LIB_DEPENDS plateau citygml xerces-c GLTFSDK crypto ssl LibXml2 httplib zlibstatic libjpeg-turbo png15_static tiff OpenMeshCore OpenMeshTools)

    if(WIN32)
      find_program(MSVC_LIB_TOOL lib.exe)
      message("msvc_lib_tool = ${MSVC_LIB_TOOL}")
      add_custom_target(plateau_combined
              ALL
              COMMAND
              ${MSVC_LIB_TOOL} /OUT:$<TARGET_FILE_DIR:plateau>\\plateau_combined.lib
              ${COMBINE_LIB_FILES}
              DEPENDS ${COMBINE_LIB_DEPENDS}
              WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
              )
    elseif(UNIX)
      # macOS で libファイル結合します。
      if(APPLE)
        add_custom_target(plateau_combined
                ALL
                COMMAND
                /usr/bin/libtool -o $<TARGET_FILE_DIR:plateau>/libplateau_combined.a ${COMBINE_LIB_FILES}
                DEPENDS ${COMBINE_LIB_DEPENDS}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                )
      else()
        # linux で libファイル結合は現状うまくいっていません。
        # 参考 : https://qiita.com/fault/items/2a3201fc697a2e537906
  #      set(ADDLIB_ARGS "")
  #      foreach(LIB ${COMBINE_LIB_FILES})
  #        message("LIB = ${LIB}")
  #        list(APPEND ADDLIB_ARGS addlib ${LIB} "\\n" )
  #      endforeach()
  #      message("addlib_args = ${ADDLIB_ARGS}")
  #      set(COMBINE_ARG "-n -e create $<TARGET_FILE_DIR:plateau>/plateau_combined.a \\n ${ADDLIB_ARGS} \\n save \\n end")
  #      message(combine_arg = ${COMBINE_ARG})
  #      add_custom_command(
  #              TARGET plateau
  #              POST_BUILD
                #COMMAND echo -n -e create $<TARGET_FILE_DIR:plateau>/plateau_combined.a \n ${ADDLIB_ARGS} \n save \n end | ar -M
  #              COMMAND echo ${COMBINE_ARG} | ar -M
  #              DEPENDS ${COMBINE_LIB_DEPENDS}
  #              WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  #      )
      endif()
    endif()
  endif()
endif()
