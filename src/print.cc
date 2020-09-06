// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/print.h"
#include "alutils/string.h"

#include <stdio.h>
#include <stdarg.h>

namespace alutils {

void default_print_none(const char* format, ...) {}

ALUTILS_PRINT_WRAPPER(default_print_debug_out, fprintf(stderr, "OUTPUT: %s\n", msg.c_str()));
ALUTILS_PRINT_WRAPPER(default_print_debug,     fprintf(stderr, "DEBUG: %s\n", msg.c_str()));
ALUTILS_PRINT_WRAPPER(default_print_info,      fprintf(stderr, "INFO: %s\n", msg.c_str()));
ALUTILS_PRINT_WRAPPER(default_print_notice,    fprintf(stderr, "NOTICE: %s\n", msg.c_str()));
ALUTILS_PRINT_WRAPPER(default_print_warn,      fprintf(stderr, "WARN: %s\n", msg.c_str()));
ALUTILS_PRINT_WRAPPER(default_print_error,     fprintf(stderr, "ERROR: %s\n", msg.c_str()));
ALUTILS_PRINT_WRAPPER(default_print_critical,  fprintf(stderr, "CRITICAL: %s\n", msg.c_str()));

print_type* print_debug_out = default_print_debug_out;
print_type* print_debug     = default_print_debug;
print_type* print_info      = default_print_info;
print_type* print_notice    = default_print_notice;
print_type* print_warn      = default_print_warn;
print_type* print_error     = default_print_error;
print_type* print_critical  = default_print_critical;

log_level_t log_level = LOG_ERROR;

} // namespace alutils
