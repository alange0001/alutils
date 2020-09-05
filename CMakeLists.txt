cmake_minimum_required(VERSION 3.2)

project(alutils LANGUAGES CXX)

#################################################################
set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(WORKSPACE "${CMAKE_CURRENT_SOURCE_DIR}/..")

# https://github.com/fmtlib/fmt.git
set(FMT_DIR "${WORKSPACE}/fmt")
include_directories("${FMT_DIR}/include")
list(APPEND THIRDPARTY_LIBS "${FMT_DIR}/build/libfmt.a")

#################################################################

include_directories("${PROJECT_DIR}/include")

add_library(alutils src/string.cc)
target_link_libraries(alutils ${THIRDPARTY_LIBS})
set_property(TARGET alutils PROPERTY CXX_STANDARD 17)

add_subdirectory(test)
