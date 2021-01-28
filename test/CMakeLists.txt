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

add_executable(command-test command-test.cc)
target_link_libraries(command-test alutils ${THIRDPARTY_LIBS})
set_property(TARGET command-test PROPERTY CXX_STANDARD 17)

add_executable(random-test random-test.cc)
target_link_libraries(random-test alutils ${THIRDPARTY_LIBS})
set_property(TARGET random-test PROPERTY CXX_STANDARD 17)

add_executable(socket-test socket-test.cc)
target_link_libraries(socket-test alutils ${THIRDPARTY_LIBS})
set_property(TARGET socket-test PROPERTY CXX_STANDARD 17)

add_executable(tmp-test tmp-test.cc)
target_link_libraries(tmp-test alutils ${THIRDPARTY_LIBS})
set_property(TARGET tmp-test PROPERTY CXX_STANDARD 17)
