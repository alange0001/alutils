// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/io.h"

#include "alutils/print.h"
#include "alutils/internal.h"
#include "alutils/string.h"

#include <stdexcept>

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ ""

const char* errno2name(int errno_) {
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

const std::string strerror2(int errno_) {
	return sprintf("[%s:%d] %s", errno2name(errno_), errno_, strerror(errno_));
}

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "Poll::"

Poll::Poll(int fd_, uint32_t timeout_ms, bool throw_except) : fd(fd_) {
	pollfd pfd{
		.fd = fd,
		.events=(POLLIN|POLLOUT|POLLRDHUP|POLLHUP|POLLERR|POLLNVAL),
		.revents=0};

	auto ret = poll(&pfd, 1, timeout_ms);
	if (ret != -1) {
		revents = pfd.revents;
		if (log_level <= LOG_DEBUG && (pfd.revents & (eof_events | error_events))) {
			PRINT_DEBUG("fd = %d, revents = %s", fd, str().c_str());
		}

	} else { // ret == -1
		errno_ = errno;
		if (errno != EAGAIN && errno != EINTR) {
			exception = true;
			if (throw_except)
				throw std::runtime_error(sprintf("poll syscall returned an error for file descriptor %d: %s", fd, strerror2(errno)).c_str());
		}
	}
}

std::string Poll::str(short filter) {
	std::string aux;
#	define P2S( pname, descr ) if (revents & filter & pname) {aux += (aux.length())?"; ":""; aux += #pname; aux += ": \"" descr "\"";}
	P2S(POLLIN,     "there is data to read");
	P2S(POLLPRI,    "");
	P2S(POLLOUT,    "writing is now possible");
	P2S(POLLRDHUP,  "stream socket peer closed connection, or shut down writing half of connection");
	P2S(POLLERR,    "error condition");
	P2S(POLLHUP,    "hang up");
	P2S(POLLNVAL,   "invalid request");
	P2S(POLLRDBAND, "priority band data can be read");
	P2S(POLLWRBAND, "priority data may be written");
#	undef P2S
	return sprintf("{%s}", aux.c_str());
}

bool Poll::is_eof() {
	return revents & eof_events;
}

bool Poll::is_error() {
	return revents & error_events;
}

} // namespace alutils
