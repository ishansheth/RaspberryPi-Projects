cmake_minimum_required(VERSION 3.0)
message(STATUS "Using Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Processor architecture: ${CMAKE_SYSTEM_PROCESSOR}")
#https://stackoverflow.com/questions/32469953/why-is-cmake-designed-so-that-it-removes-runtime-path-when-installing
set(BUILD_ALL On)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
add_subdirectory(gy80_poc)
add_subdirectory(video_stream)

