cmake_minimum_required(VERSION 2.8.9)
project(gyroPoc)

set(PROJECT_LINK_LIBS wiringPi)

set(SOURCES i2c_poc.cpp L3G4200D.cpp L3G4200D.h ADXL345.h ADXL345.cpp)

add_executable(gyroPoc ${SOURCES})

target_link_libraries(gyroPoc ${PROJECT_LINK_LIBS})
