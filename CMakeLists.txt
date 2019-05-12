cmake_minimum_required(VERSION 2.8.9)
project(gyroPoc)

set(CMAKE_BUILD_TYPE Debug)

set(PROJECT_LINK_LIBS wiringPi)

set(SOURCES_SERVER i2c_poc.cpp L3G4200D.cpp L3G4200D.h ADXL345.h ADXL345.cpp)

set(SOURCES_CLIENT client.cpp)

#add_executable(gyroPoc ${SOURCES_SERVER})

#target_link_libraries(gyroPoc ${PROJECT_LINK_LIBS})

add_executable(client ${SOURCES_CLIENT})

target_link_libraries(client pthread)

