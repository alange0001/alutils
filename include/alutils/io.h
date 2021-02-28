// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <string>

#include <poll.h>

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ ""

const char* errno2name(int errno_);

const std::string strerror2(int errno_);

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "Poll::"

struct Poll {
	int   fd;
	short revents   = 0;
	bool  exception = false;
	int   errno_    = 0;

	Poll(int fd_, uint32_t timeout_ms=0, bool throw_except=true);
	std::string str(short filter = 0xffff);
	bool is_eof();
	bool is_error();

	static const short eof_events   = POLLRDHUP | POLLHUP;
	static const short error_events = POLLERR   | POLLNVAL;
};

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ ""

} // namespace alutils
