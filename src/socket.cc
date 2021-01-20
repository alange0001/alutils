// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/print.h"
#include "alutils/internal.h"
#include "alutils/socket.h"
#include "alutils/string.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstdio>

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "SocketServer::"

SocketServer::SocketServer(const std::string& name, handler_t handler) : name(name), handler(handler) {
	PRINT_DEBUG("constructor");
	sockaddr_un s_name;

	if (name.length() > sizeof(s_name.sun_path)-1)
		throw std::runtime_error(sprintf("socket name exceeds the maximum size of %s bytes (%s)", v2s(sizeof(s_name.sun_path)), name.c_str()).c_str());

	sock = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if (sock < 0)
		throw std::runtime_error(sprintf("failed to create socket (%s)", name.c_str()).c_str());

	s_name.sun_family = AF_UNIX;
	strncpy(s_name.sun_path, name.c_str(), sizeof(s_name.sun_path) -1);
	s_name.sun_path[sizeof(s_name.sun_path) -1] = '\0';

	if (bind(sock, (struct sockaddr *) &s_name, sizeof(sockaddr_un)) < 0)
		throw std::runtime_error(sprintf("failed to bind the socket name (%s)", name.c_str()).c_str());

	thread = std::thread([this]()->void {this->thread_main();});
}

SocketServer::~SocketServer() {
	PRINT_DEBUG("destructor");
	stop_ = true;
	while (active)
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	if (thread.joinable())
		thread.join();
	remove(name.c_str());
}

void SocketServer::thread_main() noexcept {
	PRINT_DEBUG("thread_main");
	char buffer[256]; buffer[sizeof(buffer)-1] = '\n';

	active = true;
	try {
		while(! stop_) {
			auto r = recv(sock, buffer, sizeof(buffer)-1, MSG_DONTWAIT);
			if (r > 0) {
				handler(buffer);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	} catch (std::exception& e) {
		PRINT_DEBUG("exception received: %s", e.what());
		thread_exception = std::current_exception();
	}
	close(sock);
	active = false;
}

void SocketServer::stop() {
	stop_ = true;
}

bool SocketServer::status(bool throw_exception) {
	if (thread_exception && throw_exception)
		std::rethrow_exception(thread_exception);
	return active;
}

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "SocketClient::"

SocketClient::SocketClient(const std::string& name){
	PRINT_DEBUG("constructor");
	sockaddr_un s_name;
	if (name.length() > sizeof(s_name.sun_path)-1)
		throw std::runtime_error("socket name exceeds the maximum size");

	s_name.sun_family = AF_UNIX;
	strncpy(s_name.sun_path, name.c_str(), sizeof(s_name.sun_path) -1);
	s_name.sun_path[sizeof(s_name.sun_path) -1] = '\0';

	sock = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if (sock < 0)
		throw std::runtime_error("failed to create socket");

	if (connect(sock, (sockaddr*) &s_name, sizeof(s_name)) < 0)
		throw std::runtime_error("failed to connect to the socket");
}

SocketClient::~SocketClient() {
	PRINT_DEBUG("destructor");
	close(sock);
}

void SocketClient::send_msg(const std::string& str){
	PRINT_DEBUG("send message: %s", str.c_str());
	if (send(sock, str.c_str(), str.length(), 0) < 0)
		throw std::runtime_error("failed to send a message to the socket");
}

} // namespace alutils
