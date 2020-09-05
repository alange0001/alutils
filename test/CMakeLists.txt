cmake_minimum_required(VERSION 3.2)

add_executable(string-test string-test.cc)
target_link_libraries(string-test alutils ${THIRDPARTY_LIBS})
set_property(TARGET string-test PROPERTY CXX_STANDARD 17)
