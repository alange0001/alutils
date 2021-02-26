// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <thread>
#include <functional>
#include <string>
#include <atomic>
#include <vector>
#include <memory>

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "Socket::"

/**
 * Bidirectional Unix socket server and client
 */
class Socket {
public: // types:
	enum type_t {tServer, tClient};
	typedef std::function<bool(const std::string& msg, bool throw_except)> sender_t;
	struct HandlerData {
		Socket* obj;
		std::string msg;
		sender_t send;
		bool more_data = false;
	};
	typedef std::function<void(HandlerData* data)> handler_t;
	enum ErrorScope {tServerMain, tServerConnection, tServerHandler, tClientMain, tClientHandler};
	struct ErrorData {
		Socket* obj;
		ErrorScope scope;
		const std::string msg;
		std::exception_ptr exception;
	};
	typedef std::function<void(ErrorData* data)> error_handler_t;
	struct Params {
		uint32_t               buffer_size = 1024;  // buffer used to receive each message
		bool                   thread_handler = true;   // if true, spawn a new thread to call the handler for each message received
		error_handler_t server_error_handler = nullptr;
		error_handler_t client_error_handler = nullptr;
	};

private:
	type_t                     type;
	std::string                name;
	handler_t                  handler;
	int                        sock = -1;
	bool                       main_thread_ok = false;
	bool                       active = false;
	bool                       stop_ = false;
	std::thread                thread;
	std::exception_ptr         thread_exception;
	std::unique_ptr<ErrorData> error_data;
	std::atomic<int>           error_lock;
	std::atomic<int>           children;

	Params                     params;

public:
	Socket(const type_t type_, const std::string& name_, handler_t handler_);
	Socket(const type_t type_, const std::string& name_, handler_t handler_, Params params_);
	Socket(const Socket&) = delete;
	Socket operator=(const Socket&) = delete;
	~Socket();
	bool send_msg(const std::string& str, bool throw_except=true);
	bool isActive();
	std::unique_ptr<ErrorData> getError();
	void stop();

private:
	void thread_server_main() noexcept;
	void thread_server_child(int fd) noexcept;
	bool send_msg_fd(int fd, const std::string& str, bool throw_except);
	void thread_client_main() noexcept;
	void handleException(const char* function_name, error_handler_t error_handler, ErrorScope scope, const std::string& except_msg, std::exception_ptr except_ptr);
};

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ ""

} // namespace alutils
