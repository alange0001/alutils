
all: build
	cd build && make -j6

build: CMakeLists.txt 3rd-party/procps/proc/.libs/libprocps.a
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

3rd-party/procps/configure:
	mkdir 3rd-party || true
	test -d 3rd-party/procps || git clone -b "v3.3.17" --depth 1 -- https://gitlab.com/procps-ng/procps.git 3rd-party/procps
	cd 3rd-party/procps && ./autogen.sh

3rd-party/procps/proc/.libs/libprocps.a: 3rd-party/procps/configure
	cd 3rd-party/procps && ./configure --without-systemd
	cd 3rd-party/procps && make

clean-3rd-party:
	rm -fr 3rd-party

clean:
	rm -fr build
