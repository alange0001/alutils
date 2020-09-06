// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#define PRINT_DEBUG_OUT(format, ...) if (log_level <= LOG_DEBUG_OUT) print_debug("[%d] " __CLASS__ "%s() OUTPUT: " format, __LINE__, __func__, ##__VA_ARGS__)
#define PRINT_DEBUG(format, ...) if (log_level <= LOG_DEBUG) print_debug("[%d] " __CLASS__ "%s(): " format, __LINE__, __func__, ##__VA_ARGS__)
#define PRINT_INFO(format, ...) if (log_level <= LOG_INFO) print_debug(format, ##__VA_ARGS__)
#define PRINT_NOTICE(format, ...) if (log_level <= LOG_NOTICE) print_debug(format, ##__VA_ARGS__)
#define PRINT_WARN(format, ...) if (log_level <= LOG_WARN) print_debug(format, ##__VA_ARGS__)
#define PRINT_ERROR(format, ...) if (log_level <= LOG_ERROR) print_debug(format, ##__VA_ARGS__)
#define PRINT_CRITICAL(format, ...) if (log_level <= LOG_CRITICAL) print_debug(format, ##__VA_ARGS__)
