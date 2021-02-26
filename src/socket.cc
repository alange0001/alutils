// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/print.h"
#include "alutils/internal.h"
#include "alutils/socket.h"
#include "alutils/string.h"

#include <stdexcept>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ ""

static inline void sleep_ms(uint32_t ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

static int select_fd(int fd, uint32_t timeout_ms=1) {
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);
	timeval timeout {0, timeout_ms * 1000};
	return select(fd +1, &fdset, NULL, NULL, &timeout);
}

static const char* errno2name(int errno_) {
#	define E2S( ename ) if (errno_ == ename) return #ename
	E2S(EAGAIN);
	E2S(EWOULDBLOCK);
	E2S(EBADF);
	E2S(EFAULT);
	E2S(EINTR);
	E2S(EINVAL);
	E2S(EIO);
	E2S(EISDIR);
	E2S(ENOMEM);
	return "unknown";
#	undef E2S
}

static inline const std::string strerror_plus(int errno_) {
	return sprintf("[%s:%d] %s", errno2name(errno_), errno_, strerror(errno_));
}

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "Socket::"

#define Type2Str type==tServer?"SERVER":"CLIENT"

Socket::Socket(const type_t type_, const std::string& name_, handler_t handler_) : Socket(type_, name_, handler_, Params()) {}

Socket::Socket(const type_t type_, const std::string& name_, handler_t handler_, Params params_) :
               type(type_), name(name_), handler(handler_), params(params_), error_lock(0), children(0)
{
	PRINT_DEBUG("%s: constructor", Type2Str);
	PRINT_DEBUG("%s:\tname=%s", Type2Str, name.c_str());
	PRINT_DEBUG("%s:\tbuffer_size=%s", Type2Str, v2s(params.buffer_size));
	PRINT_DEBUG("%s:\tthread_handler=%s", Type2Str, v2s(params.thread_handler));
	sockaddr_un s_name;

	if (type != tServer && type != tClient)
		throw std::runtime_error("invalid type for the class Socket");

	if (name.length() > sizeof(s_name.sun_path)-1)
		throw std::runtime_error(sprintf("socket name exceeds the maximum size of %s bytes (%s)", v2s(sizeof(s_name.sun_path)), name.c_str()).c_str());

	s_name.sun_family = AF_UNIX;
	strncpy(s_name.sun_path, name.c_str(), sizeof(s_name.sun_path) -1);
	s_name.sun_path[sizeof(s_name.sun_path) -1] = '\0';

	sock = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (sock == -1)
		throw std::runtime_error(sprintf("failed to create the socket \"%s\": %s", name.c_str(), strerror_plus(errno).c_str()).c_str());
	PRINT_DEBUG("%s: sock = %d", Type2Str, sock);

	if (type == tServer) {
		PRINT_DEBUG("%s: bind socket name \"%s\"", Type2Str, name.c_str());
		if (bind(sock, (struct sockaddr *) &s_name, sizeof(sockaddr_un)) == -1)
			throw std::runtime_error(sprintf("failed to bind the socket name \"%s\": %s", name.c_str(), strerror_plus(errno).c_str()).c_str());
		PRINT_DEBUG("%s: listening the socket named \"%s\"", Type2Str, name.c_str());
		if (listen(sock, 5) == -1)
			throw std::runtime_error(sprintf("failed to listen the socket name \"%s\": %s", name.c_str(), strerror_plus(errno).c_str()).c_str());

		PRINT_DEBUG("%s: initiating the main server thread", Type2Str);
		thread = std::thread(&Socket::thread_server_main, this);
		while (! main_thread_ok)
			sleep_ms(100);
	} else {
		PRINT_DEBUG("%s: connecting to the socket named \"%s\"", Type2Str, name.c_str());
		if (connect(sock, (sockaddr*) &s_name, sizeof(s_name)) == -1)
			throw std::runtime_error(sprintf("failed to connect to the socket \"%s\": %s", name.c_str(), strerror_plus(errno).c_str()).c_str());

		PRINT_DEBUG("%s: initiating the client thread", Type2Str);
		thread = std::thread(&Socket::thread_client_main, this);
		while (! main_thread_ok)
			sleep_ms(100);
	}
	PRINT_DEBUG("%s: constructor finished", Type2Str);
}

Socket::~Socket() {
	PRINT_DEBUG("%s: destructor", Type2Str);
	stop_ = true;
	for (int i = 0; i < 30 && (active || children.load() > 0); i++) {
		PRINT_DEBUG("%s: i=%d, active=%s, children=%d", Type2Str, i, active?"true":"false", children.load());
		sleep_ms(100);
	}
	if (thread.joinable()) {
		PRINT_DEBUG("%s: join main thread", Type2Str);
		thread.join();
	}
	if (type == tServer) {
		PRINT_DEBUG("%s: remove socket %s", Type2Str, name.c_str());
		remove(name.c_str());
	}
	PRINT_DEBUG("%s: destructor finished", Type2Str);
}

void Socket::thread_server_main() noexcept {
	main_thread_ok = true;
	active = true;
	PRINT_DEBUG("%s: thread_server_main", Type2Str);

	try {
		sockaddr client_addr;
		socklen_t client_addr_size = sizeof(client_addr);

		fd_set fdset;
		FD_ZERO(&fdset);
		timeval timeout {0,1000};

		while(!stop_) {
			FD_SET(sock, &fdset);
			auto r = select(sock +1, &fdset, NULL, NULL, &timeout);

			if (stop_) break;
			if (r > 0) {
				auto fd = accept4(sock, &client_addr, &client_addr_size, SOCK_NONBLOCK);
				if (stop_) break;
				if (fd < 0) {
					throw std::runtime_error(sprintf("accept4 returned error for the socket \"%s\": %s",name.c_str(), strerror_plus(errno).c_str()).c_str());
				}

				PRINT_DEBUG("%s: Connection received (fd=%d). Creating child thread.", Type2Str, fd);
				std::thread child([this, fd]()->void{this->thread_server_child(fd);});
				child.detach();

			} else if (r == -1) {
				throw std::runtime_error(sprintf("select syscall returned error for the socket \"%s\": %s", name.c_str(), strerror_plus(errno).c_str()).c_str());
			}

			sleep_ms(200);
		}
	} catch (std::exception& e) {
		handleException(__func__, params.server_error_handler, tServerMain, e.what(), std::current_exception());
	}
	close(sock);
	active = false;
	PRINT_DEBUG("%s: thread_server_main finished", Type2Str);
}

void Socket::thread_server_child(int fd) noexcept {
	children++;
	PRINT_DEBUG("%s: thread_server_child, fd=%d", Type2Str, fd);
	std::atomic<int> handler_count = 0;

	try {
		auto sender = [this,fd](const std::string& msg, bool throw_except)->bool{return this->send_msg_fd(fd, msg, throw_except);};

		std::unique_ptr<char> buffer(new char[params.buffer_size+1]); buffer.get()[params.buffer_size] = '\0';

		while(!stop_ && active) {
			std::unique_ptr<HandlerData> data(new HandlerData{.obj=this, .send=sender});

			auto r = select_fd(fd, 0);

			if (stop_ || !active) break;
			if (r > 0) {
				auto r2 = read(fd, buffer.get(), params.buffer_size);
				if (r2 >= 0) {
					buffer.get()[r2] = '\0';
					data->msg = buffer.get();
				}
				if (stop_ || !active) break;

				if (r2 > 0){
					PRINT_DEBUG("%s: message received: %s", Type2Str, data->msg.c_str());
					data->more_data = (select_fd(sock, 0) > 0);

					if (handler) {
						if (params.thread_handler) {
							PRINT_DEBUG("%s: swap buffers", Type2Str);
							HandlerData* handler_data = data.release();
							handler_count++;
							PRINT_DEBUG("%s: initiating handler thread", Type2Str);
							std::thread handler_thread([this, handler_data, &handler_count]()->void{
								try {
									handler(handler_data);
								} catch (std::exception& e) {
									handleException("thread_server_child.handler_thread", params.server_error_handler, tServerHandler, e.what(), std::current_exception());
								}
								delete handler_data;
								handler_count--;
							});
							handler_thread.detach();
						} else {
							try {
								handler(data.get());
							} catch (std::exception& e) {
								handleException(__func__, params.server_error_handler, tServerHandler, e.what(), std::current_exception());
							}
						}
						if (stop_ || !active) break;
					}
				} else if (r2 == 0) {
					PRINT_DEBUG("%s: end of file", Type2Str);
					break;
				} else if (r2 == -1) {
					if (errno != EAGAIN) {
						throw std::runtime_error(sprintf("failed to read data from the socket \"%s\", connection %d: %s", name.c_str(), fd, strerror_plus(errno).c_str()).c_str());
					}
				}

			} else if (r == -1) {
				throw std::runtime_error(sprintf("select syscall returned error for the socket \"%s\", connection %d: %s", name.c_str(), fd, strerror_plus(errno).c_str()).c_str());
			}

			sleep_ms(200);
		}
	} catch (std::exception& e) {
		handleException(__func__, params.server_error_handler, tServerConnection, e.what(), std::current_exception());
	}
	close(fd);
	for (int i=0; i < 20 && handler_count.load() > 0; i++) {
		PRINT_DEBUG("%s: i=%d, handler_count=%d", Type2Str, i, handler_count.load());
		sleep_ms(100);
	}
	children--;
	PRINT_DEBUG("%s: thread_server_child finished (fd=%d)", Type2Str, fd);
}

bool Socket::send_msg_fd(int fd, const std::string& str, bool throw_except){
	PRINT_DEBUG("%s: send message: %s", Type2Str, str.c_str());
	if (send(fd, str.c_str(), str.length(), MSG_CONFIRM) == -1) {
		if (throw_except)
			throw std::runtime_error(sprintf("failed to send a message to the socket \"%s\", connection %d: %s", name.c_str(), fd, strerror_plus(errno).c_str()).c_str());
		else
			return false;
	}
	return true;
}

bool Socket::send_msg(const std::string& str, bool throw_except){
	PRINT_DEBUG("%s: send_msg", Type2Str);
	if (type != tClient)
		throw std::runtime_error("only client instances may use the method send_msg");

	PRINT_DEBUG("%s: send message: %s", Type2Str, str.c_str());
	return send_msg_fd(sock, str, throw_except);
}

void Socket::thread_client_main() noexcept {
	main_thread_ok = true;
	active = true;
	PRINT_DEBUG("%s: thread_client_main", Type2Str);

	auto sender = [this](const std::string& msg, bool throw_except)->bool{return this->send_msg(msg, throw_except);};
	std::atomic<int> handler_count = 0;

	try {
		std::unique_ptr<HandlerData> data;
		std::unique_ptr<char> buffer(new char[params.buffer_size+1]); buffer.get()[params.buffer_size] = '\0';

		while(! stop_) {
			data.reset(new HandlerData{.obj=this, .send=sender});

			auto r = recv(sock, buffer.get(), params.buffer_size, MSG_DONTWAIT);
			if (r >= 0) {
				buffer.get()[r] = '\0';
				data->msg = buffer.get();
			}
			if (stop_) break;

			if (r > 0 && handler) {
				//PRINT_DEBUG("%s: msg = %s", Type2Str, data->msg.c_str());
				data->more_data = (select_fd(sock, 0) > 0);
				while (data->more_data) {
					auto r_more = recv(sock, buffer.get(), params.buffer_size, MSG_DONTWAIT);
					if (r_more >= 0) {
						buffer.get()[r_more] = '\0';
						data->msg += buffer.get();
					}
					//PRINT_DEBUG("%s: msg = %s", Type2Str, data->msg.c_str());
					data->more_data = (select_fd(sock, 0) > 0);
				}
				if (stop_) break;

				if (params.thread_handler) {
					HandlerData* handler_data = data.release();
					//PRINT_DEBUG("%s: release data from unique_ptr. msg = %s", Type2Str, handler_data->msg.c_str());
					handler_count++;
					PRINT_DEBUG("%s: initiating handler thread", Type2Str);
					std::thread handler_thread([this, handler_data, &handler_count]()->void{
						try {
							handler(handler_data);
						} catch (std::exception& e) {
							handleException("thread_client_main.handler_thread", params.client_error_handler, tClientHandler, e.what(), std::current_exception());
						}
						delete handler_data;
						handler_count--;
					});
					handler_thread.detach();
				} else {
					try {
						handler(data.get());
					} catch (std::exception& e) {
						handleException(__func__, params.client_error_handler, tClientHandler, e.what(), std::current_exception());
					}
				}
				if (stop_) break;
			} else if (r == -1) {
				if (errno != EAGAIN) {
					PRINT_ERROR("%s: recv error: %s", Type2Str, strerror_plus(errno).c_str());
				}
			}

			sleep_ms(200);
		}
	} catch (std::exception& e) {
		handleException(__func__, params.client_error_handler, tClientMain, e.what(), std::current_exception());
	}

	close(sock);
	for (int i=0; i < 20 && handler_count.load() > 0; i++) {
		PRINT_DEBUG("%s: i=%d, handler_count=%d", Type2Str, i, handler_count.load());
		sleep_ms(100);
	}
	active = false;
	PRINT_DEBUG("%s: thread_client_main finished", Type2Str);
}

bool Socket::isActive() {
	return active;
}

void Socket::handleException(const char* function_name, error_handler_t error_handler, ErrorScope scope, const std::string& except_msg, std::exception_ptr except_ptr) {
	PRINT_DEBUG("%s: handling error data: function=%s, scope=%d, msg=%s", Type2Str, function_name, scope, except_msg.c_str());

	auto store_error = new ErrorData{.scope = scope, .msg = except_msg, .exception = except_ptr};

	/*lock*/   int zero = 0; while (! error_lock.compare_exchange_weak(zero, 1)) zero = 0;
	error_data.reset(store_error);
	/*unlock*/ error_lock.store(0);

	PRINT_DEBUG("%s: error_data = %p", Type2Str, error_data.get());

	if (error_handler != nullptr) {
		PRINT_DEBUG("%s: handler exception: %s", Type2Str, except_msg.c_str());
		ErrorData data = {.obj=this, .scope = scope, .msg = except_msg, .exception = except_ptr};
		error_handler(&data);
	} else {
		PRINT_ERROR("exception: %s", except_msg.c_str());
	}
}

std::unique_ptr<Socket::ErrorData> Socket::getError() {
	std::unique_ptr<ErrorData> ret;

	/*lock*/   int zero = 0; while (! error_lock.compare_exchange_weak(zero, 1)) zero = 0;
	ret.swap(error_data);
	/*unlock*/ error_lock.store(0);

	PRINT_DEBUG("%s: loading error data: scope=%d, msg=%s", Type2Str, ret.get()!=nullptr?ret->scope:-1, ret.get()!=nullptr?ret->msg.c_str():"NULL");
	return ret;
}

void Socket::stop() {
	stop_ = true;
}

} // namespace alutils
