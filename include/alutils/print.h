// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <functional>

namespace alutils {

typedef enum {
	LOG_DEBUG_OUT,
	LOG_DEBUG,
	LOG_INFO,
	LOG_NOTICE,
	LOG_WARN,
	LOG_ERROR,
	LOG_CRITICAL
} log_level_t;

extern log_level_t log_level;

void default_print_none(const char* format, ...);

typedef decltype(default_print_none) print_type;

extern print_type* print_debug_out;
extern print_type* print_debug;
extern print_type* print_info;
extern print_type* print_notice;
extern print_type* print_warn;
extern print_type* print_error;
extern print_type* print_critical;

#define ALUTILS_PRINT_WRAPPER(name, function) \
void name(const char* format, ...) {         \
	va_list args; va_start(args, format);    \
	std::string msg = vsprintf(format, args);  \
	function;                                \
	va_end(args);                            \
}

} // namespace alutils
