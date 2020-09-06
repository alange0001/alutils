
all: build
	cd build && make -j6

build: CMakeLists.txt
	mkdir build || true
	cd build && cmake .. && make clean

test: all
	build/test/string-test
	build/test/print-test
	build/test/process-test

clean:
	cd build && make clean
