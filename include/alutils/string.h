// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <string>
#include <vector>
#include <map>
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

////////////////////////////////////////////////////////////////////////////////////
extern bool debug_parse;

template<typename T>
T parse(const std::string &value,
        const bool required=true, const T default_=(T)0, const char* error_msg=nullptr,
        std::function<bool(T)> check_method=nullptr );

#define DECLARE_PARSE(NAME, TYPE)                                                         \
    inline TYPE NAME(                                                                     \
        const std::string &value, const bool required=true, const TYPE default_=(TYPE)0,  \
        const char* error_msg=nullptr, std::function<bool(TYPE)> check_method=nullptr )   \
    {                                                                                     \
        return parse<TYPE>(value, required, default_, error_msg, check_method);           \
    }

DECLARE_PARSE(parseBool,   bool);
DECLARE_PARSE(parseInt32,  int32_t);
DECLARE_PARSE(parseInt64,  int64_t);
DECLARE_PARSE(parseUint32, uint32_t);
DECLARE_PARSE(parseUint64, uint64_t);
DECLARE_PARSE(parseDouble, double);

#undef DECLARE_PARSE

////////////////////////////////////////////////////////////////////////////////////
extern bool debug_parseSuffix;

template <typename T>
T parseSuffix(const std::string& value, const std::map<std::string, T>& suffixes);

#define DECLARE_PARSE_SUFFIX(NAME, TYPE)                                                    \
    inline TYPE NAME(const std::string& value, const std::map<std::string, TYPE>& suffixes) \
    {                                                                                       \
        return parseSuffix<TYPE>(value, suffixes);                                          \
    }

DECLARE_PARSE_SUFFIX(parseInt32Suffix,  int32_t);
DECLARE_PARSE_SUFFIX(parseInt64Suffix,  int64_t);
DECLARE_PARSE_SUFFIX(parseUint32Suffix, uint32_t);
DECLARE_PARSE_SUFFIX(parseUint64Suffix, uint64_t);
DECLARE_PARSE_SUFFIX(parseDoubleSuffix, double  );

#undef DECLARE_PARSE_SUFFIX

////////////////////////////////////////////////////////////////////////////////////
std::string vsprintf(const char* format, va_list args);
std::string sprintf(const char* format, ...);

} // namespace alutils
