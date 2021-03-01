cmake_minimum_required(VERSION 3.2)

project(alutils LANGUAGES CXX)

#################################################################
set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(WORKSPACE "${CMAKE_CURRENT_SOURCE_DIR}/..")

list(APPEND THIRDPARTY_LIBS pthread)
list(APPEND THIRDPARTY_LIBS dl)
list(APPEND THIRDPARTY_LIBS m)

# 3rd-party/procps:
#   Source code: https://gitlab.com/procps-ng/procps.git
#   Download and compilation routines are in Makefile.
set(PROCPS_DIR "${PROJECT_DIR}/3rd-party/procps")
include_directories("${PROCPS_DIR}")
list(APPEND THIRDPARTY_LIBS ${PROCPS_DIR}/proc/.libs/libprocps.a)

#################################################################

include_directories("${PROJECT_DIR}/include")

add_library(alutils src/string.cc src/print.cc src/process.cc src/command.cc src/random.cc src/socket.cc src/io.cc)
target_link_libraries(alutils ${THIRDPARTY_LIBS})
set_property(TARGET alutils PROPERTY CXX_STANDARD 17)
set_property(TARGET alutils PROPERTY POSITION_INDEPENDENT_CODE ON)

add_subdirectory(test)
