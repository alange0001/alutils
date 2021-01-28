
all: build
	cd build && make -j6

build: CMakeLists.txt
	mkdir build || true
	cd build && cmake .. && make clean

string-test: all
	build/test/string-test

print-test: all
	build/test/print-test

process-test: all
	build/test/process-test

command-test: all
	build/test/command-test

random-test: all
	build/test/random-test

socket-test: all
	build/test/socket-test

tmp-test: all
	build/test/tmp-test

test: string-test print-test process-test command-test random-test socket-test

clean:
	cd build && make clean
