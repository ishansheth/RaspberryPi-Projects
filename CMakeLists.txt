cmake_minimum_required(VERSION 2.8.9)
project(gyroPoc)

set(CMAKE_BUILD_TYPE Debug)

# build gyro data server only for the RaspberryPi armv7l 
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
#set(PROJECT_LINK_LIBS wiringPi)

#  add_library(gyrocomm SHARED L3G4200D.cpp L3G4200D.h ADXL345.h ADXL345.cpp)

#  set(SOURCES_SERVER i2c_poc.cpp)

#  add_executable(sensorServer ${SOURCES_SERVER})

#  target_link_libraries(sensorServer gyrocomm wiringPi)

endif()

set(SOURCES_CLIENT client.cpp)

add_executable(client ${SOURCES_CLIENT})

add_executable(epollServer epoll_server.cpp)

add_executable(epollClient epoll_client.cpp)

target_link_libraries(client pthread)

