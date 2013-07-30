cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../addons/roxlu/lib/build/cmake/CMakeLists.txt) # roxlu cmake
get_filename_component(app_path ${CMAKE_CURRENT_LIST_DIR}/../../ REALPATH)  # get the directory name
get_filename_component(app_name ${app_path} NAME) # set directory name as application name

roxlu_add_addon(UV)
roxlu_add_addon(Utils)
roxlu_add_addon(Log)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../../src/videoencoder/include)

roxlu_app_initialize(${app_name})
   # ---------------------------------------------
   roxlu_app_add_source_file(main.cpp)
   roxlu_app_add_source_file(videoencoder/src/VideoEncoder.cpp)
   roxlu_app_add_source_file(videoencoder/src/VideoEncoderTypes.cpp)
   roxlu_app_add_source_file(videoencoder/src/VideoEncoderServerIPC.cpp)
   roxlu_app_add_source_file(videoencoder/src/VideoEncoderClientIPC.cpp)
   roxlu_app_add_source_file(videoencoder/src/VideoEncoderService.cpp)
   # ---------------------------------------------
roxlu_install_app()



# Add some tests
# ---------------------------------------------------
set(sd ${CMAKE_CURRENT_LIST_DIR}/../../src/tests/)
set(bd ${CMAKE_CURRENT_LIST_DIR}/../../src/)

set(video_encoder_sources 
  ${bd}videoencoder/src/VideoEncoder.cpp
  ${bd}videoencoder/src/VideoEncoderTypes.cpp
  ${bd}videoencoder/src/VideoEncoderServerIPC.cpp
  ${bd}videoencoder/src/VideoEncoderClientIPC.cpp
  ${bd}videoencoder/src/VideoEncoderService.cpp
)

add_executable(test_client ${sd}/test_client.cpp ${video_encoder_sources} ${roxlu_lib_source_files})
target_link_libraries(test_client ${roxlu_libs})