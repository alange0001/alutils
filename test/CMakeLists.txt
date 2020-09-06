cmake_minimum_required(VERSION 3.2)

add_executable(string-test string-test.cc)
target_link_libraries(string-test alutils ${THIRDPARTY_LIBS})
set_property(TARGET string-test PROPERTY CXX_STANDARD 17)

add_executable(print-test print-test.cc)
target_link_libraries(print-test alutils ${THIRDPARTY_LIBS})
set_property(TARGET print-test PROPERTY CXX_STANDARD 17)

add_executable(process-test process-test.cc)
target_link_libraries(process-test alutils ${THIRDPARTY_LIBS})
set_property(TARGET process-test PROPERTY CXX_STANDARD 17)
