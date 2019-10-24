cmake_minimum_required(VERSION 3.0)

#https://bytefreaks.net/programming-2/cc-how-do-you-set-gdb-debug-flag-g-with-cmake
find_package(OpenCV REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

# TODO: try find_package for the library so that it does not have to be built all the time
#include(build_commlib.cmake)

if(${BUILD_ALL})
  include(build_commlib.cmake)
  set(COMMLIB commlib)
else()
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/rpi_cmake)

  find_package(commlib REQUIRED)
  set(COMMLIB ${COMMLIB_LIBRARIES})
endif()

add_executable(vidServer video_renderer_server.cxx)

target_link_libraries(vidServer ${OpenCV_LIBS} pthread ${COMMLIB})

add_executable(vidClient video_renderer_client.cxx)

target_link_libraries(vidClient ${OpenCV_LIBS} pthread ${COMMLIB})

add_executable(matPOC opencv_matrix_poc.cxx)

target_link_libraries(matPOC ${OpenCV_LIBS} pthread)

#https://stackoverflow.com/questions/34443128/cmake-install-targets-in-subdirectories
install(TARGETS vidServer vidClient
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
