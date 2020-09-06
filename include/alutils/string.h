// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

namespace alutils {

extern const char* strip_default;

std::string& inplace_strip(std::string& src, const char* to_strip=strip_default);

inline std::string strip(const std::string& src, const char* to_strip=strip_default) {
	std::string ret = src;
	return inplace_strip(ret, to_strip);
}

std::string str_replace(const std::string& src, const char find, const char replace);

std::string& str_replace(std::string& dest, const std::string& src, const char find, const char replace);

int split_columns(std::vector<std::string>& ret, const char* str, const char* prefix=nullptr);

std::vector<std::string> split_str(const std::string& str, const std::string& delimiter);

bool     parseBool(const std::string &value, const bool required=true, const bool default_=true,
               const char* error_msg="invalid value (boolean)",
			   std::function<bool(bool)> check_method=nullptr );

uint32_t parseUint32(const std::string &value, const bool required=true, const uint32_t default_=0,
               const char* error_msg="invalid value (uint64)",
			   std::function<bool(uint32_t)> check_method=nullptr );

uint64_t parseUint64(const std::string &value, const bool required=true, const uint64_t default_=0,
               const char* error_msg="invalid value (uint64)",
			   std::function<bool(uint64_t)> check_method=nullptr );

double   parseDouble(const std::string &value, const bool required=true, const double default_=0.0,
               const char* error_msg="invalid value (double)",
			   std::function<bool(double)> check_method=nullptr );

std::string vsprintf(const char* format, va_list args);
std::string sprintf(const char* format, ...);

} // namespace alutils
