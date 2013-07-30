cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/../../../addons/roxlu/lib/build/cmake/CMakeLists.txt) # roxlu cmake
get_filename_component(app_path ${CMAKE_CURRENT_LIST_DIR}/../../ REALPATH)  # get the directory name
get_filename_component(app_name ${app_path} NAME) # set directory name as application name

roxlu_add_addon(YouTube)
roxlu_add_addon(Log)
roxlu_add_addon(Jansson)
roxlu_add_addon(Utils)
roxlu_add_addon(PCRE)

if(WIN32) 
  roxlu_add_dll(jansson.dll)
  roxlu_add_extern_lib(jansson.lib)
endif()

roxlu_app_initialize(${app_name})
   # ---------------------------------------------
   roxlu_app_add_source_file(main.cpp)
   roxlu_app_add_source_file(YouTubeService.cpp)
   # ---------------------------------------------
roxlu_install_app()



# ---------------------------------------------------------------------

   # test: Writing to a disconnected IPC server
   set(sd ${CMAKE_CURRENT_LIST_DIR}/../../src)
   add_executable(test_disconnected_ipc ${sd}/tests/test_disconnected_ipc.cpp ${roxlu_lib_source_files})
   target_link_libraries(test_disconnected_ipc ${roxlu_libs})
   
   # test: Upload incorrect json should return a 400 status code which must return a -false-
   add_executable(test_upload_wrong_json ${sd}/tests/test_upload_wrong_json.cpp ${roxlu_lib_source_files})
   target_link_libraries(test_upload_wrong_json ${roxlu_libs})
   
   # test: Upload progress information
   add_executable(test_upload_progress ${sd}/tests/test_upload_progress.cpp ${roxlu_lib_source_files})
   target_link_libraries(test_upload_progress ${roxlu_libs})

# ---------------------------------------------------------------------