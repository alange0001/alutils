// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <thread>
#include <functional>
#include <string>

namespace alutils {

class SocketServer {
	typedef std::function<void(const std::string& str)> handler_t;
	std::string        name;
	handler_t          handler;
	int                sock;
	bool               active = false;
	bool               stop_ = false;
	std::thread        thread;
	std::exception_ptr thread_exception;

	public:
	SocketServer(const std::string& name, handler_t handler);
	SocketServer(const SocketServer&) = delete;
	SocketServer operator=(const SocketServer&) = delete;
	~SocketServer();
	void stop();
	bool status(bool throw_exception=false);

	private:
	void thread_main() noexcept;
};

class SocketClient {
	std::string name;
	int sock;

	public:
	SocketClient(const std::string& name);
	~SocketClient();
	void send_msg(const std::string& str);
};

} // namespace alutils
