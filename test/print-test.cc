// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include <alutils/print.h>
#include <alutils/string.h>

#include <stdio.h>
#include <stdarg.h>

using namespace alutils;

ALUTILS_PRINT_WRAPPER(print2, printf("PRINT2: %s\n", msg.c_str()));

int main(int argc, char** argv) {
	log_level = LOG_DEBUG_OUT;

	print_debug_out("test %d %d %d", 1, 2, 3);
	print_debug    ("test %d %d %d", 1, 2, 3);
	print_info     ("test %d %d %d", 1, 2, 3);
	print_notice   ("test %d %d %d", 1, 2, 3);
	print_warn     ("test %d %d %d", 1, 2, 3);
	print_error    ("test %d %d %d", 1, 2, 3);
    print_critical ("test %d %d %d", 1, 2, 3);

    print_debug = print2;
	print_debug    ("test2 %d %d %d", 1, 2, 3);

	return 0;
}
