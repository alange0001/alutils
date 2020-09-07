// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <string>
#include <thread>
#include <functional>
#include <vector>

#include <sched.h>

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ ""

bool monitor_fgets (char* buffer, int buffer_size, std::FILE* file, bool* stop, uint64_t interval=300);

std::string command_output(const char* cmd);

std::vector<pid_t> get_children(pid_t parent_pid);

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "ProcessController::"

class ProcessController {
	std::string name;

	bool must_stop      = false;
	bool program_active = false;

	pid_t        pid     = 0;
	std::FILE*   f_stdin  = nullptr;
	std::FILE*   f_stdout = nullptr;
	std::FILE*   f_stderr = nullptr;

	std::thread        thread_stdout;
	bool               thread_stdout_active  = false;
	std::thread        thread_stderr;
	bool               thread_stderr_active  = false;
	std::exception_ptr thread_exception;

	std::function<void(const char*)> handler_stdout;
	std::function<void(const char*)> handler_stderr;

	void threadStdout() noexcept;
	void threadStderr() noexcept;
	bool checkStatus() noexcept;

	public: //---------------------------------------------------------------------
	ProcessController(const char* name_, const char* cmd,
			std::function<void(const char*)> handler_stdout_=ProcessController::default_stdout_handler,
			std::function<void(const char*)> handler_stderr_=ProcessController::default_stderr_handler);
	~ProcessController();

	bool puts(const std::string value) noexcept;

	bool isActive(bool throwexcept=false);
	int  exit_code      = 0;
	int  signal         = 0;

	static void default_stderr_handler(const char* v) { std::fputs(v, stderr); }
	static void default_stdout_handler(const char* v) { std::fputs(v, stdout); }
};

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ ""

} // namespace alutils
