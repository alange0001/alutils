// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include <alutils/print.h>
#include <alutils/process.h>

#include <chrono>
#include <thread>
#include <atomic>
#include <stdexcept>

#include <stdio.h>

using namespace alutils;

void thread_test(ThreadController::stop_t stop) {
	fprintf(stderr, "thread_test loop ");
	while (!stop()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
		fprintf(stderr, ".");
	}
	fprintf(stderr, " stopped!\n");
}

void thread_test2(ThreadController::stop_t stop) {
	fprintf(stderr, "thread_test loop ");
	while (!stop()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
		fprintf(stderr, ".");
		throw std::runtime_error("sending an exception");
	}
	fprintf(stderr, " stopped!\n");
}

int main(int argc, char** argv) {
	printf("\n\n=====================\nprocess-test:\n");
	{
		log_level = LOG_DEBUG_OUT;

		auto out = command_output("ls / |head -n 3");

		ProcessController proc(
					"ping",
					"ping 127.0.0.1 -c 1");
		while (proc.isActive()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		ThreadController thread(thread_test);
		while (thread.isActive()) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			thread.stop();
		}
		ThreadController thread2(thread_test2);
		try {
			while (thread2.isActive()) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		} catch (std::exception& e) {printf("Expected exception: %s\n", e.what());}

	}
	printf("OK!!\n");
	return 0;
}
