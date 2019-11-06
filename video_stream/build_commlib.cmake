cmake_minimum_required(VERSION 3.0)

#https://stackoverflow.com/questions/17511496/how-to-create-a-shared-library-with-cmake

project(NETCOMMLIB VERSION 1.0.1 DESCRIPTION "mylib description")

add_definitions(-std=c++11)

project(VideoStreaming)


add_library(commlib SHARED
  SensorSocket.hxx
  SensorSocket.cxx
  SensorSocketTCP.hxx
  SensorSocketTCP.cxx
  SensorSocketUDP.hxx
  SensorSocketUDP.cxx
  GyroDataSource.hxx
  GyroDataSource.cxx)

set_target_properties(commlib PROPERTIES SOVERSION 1)

install(TARGETS commlib
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

