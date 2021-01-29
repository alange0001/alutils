// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include <alutils/socket.h>
#include <alutils/print.h>
#include <chrono>

using namespace alutils;

void server_handler(Socket::HandlerData* data) {
	printf("SERVER: string received: %s \n", data->msg.c_str());
	data->send("message received!", true);
	//std::this_thread::sleep_for(std::chrono::milliseconds(200));
	//throw std::runtime_error("test");
}

void server_error(Socket::ErrorData* data) {
	printf("exception handler: msg=%s\n", data->msg.c_str());
}

void client_handler(Socket::HandlerData* data) {
	printf("CLIENT: string received: %s \n", data->msg.c_str());
}

//#define TEST1
#ifdef TEST1

std::unique_ptr<int> a(new int{3});

std::unique_ptr<int> f() {
	std::unique_ptr<int> ret;
	ret.swap(a);
	return ret;
}

#endif


int main(int argc, char** argv) {
	printf("\n\n=====================\nsocket-test:\n");
	log_level = LOG_DEBUG;

#	ifdef TEST1
	printf("a=%p\n", a.get());

	std::unique_ptr<int> b = f();
	std::unique_ptr<int> c;

	printf("a=%p, b=%p, c=%p\n", a.get(), b.get(), c.get());

	c.swap(b);
	printf("a=%p, b=%p, c=%p\n", a.get(), b.get(), c.get());

	exit(0);
#	endif

	const char* socket_name = "/tmp/alutils-socketserver.socket";

	Socket server(Socket::tServer, socket_name, server_handler, Socket::Params{.buffer_size=2048, .server_error_handler=server_error});
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	Socket client(Socket::tClient, socket_name, client_handler, Socket::Params{.buffer_size=2048});
	client.send_msg("test1");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	client.send_msg("test2");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	auto e = server.getError();
	if (e.get() != nullptr) {
		printf("getError(): scope=%d, msg=%s\n", e->scope, e->msg.c_str());
	} else {
		printf("getError(): NULL\n");
	}

	printf("OK!!\n");
	return 0;
}
