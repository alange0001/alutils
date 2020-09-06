// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include <alutils/print.h>
#include <alutils/process.h>

#include <chrono>
#include <thread>

using namespace alutils;

int main(int argc, char** argv) {
	log_level = LOG_DEBUG_OUT;

	auto out = command_output("ls /");

	ProcessController proc(
				"ping",
				"ping 127.0.0.1 -c 3");
	while (proc.isActive()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	return 0;
}
