// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include <alutils/socket.h>
#include <alutils/print.h>
#include <chrono>

using namespace alutils;

void handler(const std::string& str) {
	printf("String received via socket: %s \n", str.c_str());
}

int main(int argc, char** argv) {
	printf("\n\n=====================\nsocket-test:\n");
	log_level = LOG_DEBUG;

	const char* socket_name = "/tmp/alutils-socketserver.socket";

	SocketServer server(socket_name, handler);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	SocketClient client(socket_name);
	client.send_msg("test1");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	client.send_msg("test2");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	printf("OK!!\n");
	return 0;
}
