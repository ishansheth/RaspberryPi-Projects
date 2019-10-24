cmake_minimum_required(VERSION 3.0)

find_library(COMMLIB_LIBRARY NAMES libcommlib.so PATHS ${CMAKE_SOURCE_DIR}/install/lib)

message(STATUS "COMMLIB_LIBRARY : ${COMMLIB_LIBRARY}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(COMMLIB DEFAULT_MSG COMMLIB_LIBRARY)

mark_as_advanced(COMMLIB_LIBRARY)

set(COMMLIB_LIBRARIES ${COMMLIB_LIBRARY})
