// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/string.h"

#include <regex>
#include <stdexcept>
#include <memory>

#include <stdarg.h>

namespace alutils {

const char* strip_default = " \t\n\r\f\v";

std::string& inplace_strip(std::string& src, const char* to_strip) {
	src.erase(src.find_last_not_of(to_strip) +1);
	src.erase(0, src.find_first_not_of(to_strip));
	return src;
}

std::string str_replace(const std::string& src, const char find, const char replace) {
	std::string dest = src;
	std::replace(dest.begin(), dest.end(), find, replace);
	return dest;
}

std::string& str_replace(std::string& dest, const std::string& src, const char find, const char replace) {
	dest = src;
	std::replace(dest.begin(), dest.end(), find, replace);
	return dest;
}

int split_columns(std::vector<std::string>& ret, const char* str, const char* prefix) {
	std::cmatch cm;
	auto flags = std::regex_constants::match_any;
	std::string str_aux;

	ret.clear();

	if (prefix != nullptr) {
		std::string aux = prefix; aux += "\\s+(.+)";
		std::regex_search(str, cm, std::regex(aux.c_str()), flags);
		if (cm.size() < 2)
			return 0;

		str_aux = cm[1].str();
		str = str_aux.c_str();
	}

	for (const char* i = str;;) {
		std::regex_search(i, cm, std::regex("([^\\s]+)\\s*(.*)"), flags);
		if (cm.size() >= 3) {
			ret.push_back(cm[1].str());
			i = cm[2].first;
		} else {
			break;
		}
	}

	return ret.size();
}

std::vector<std::string> split_str(const std::string& str, const std::string& delimiter) {
	std::vector<std::string> ret;
	std::string aux = str;
	auto pos = std::string::npos;

	while ((pos = aux.find(delimiter)) != std::string::npos) {
		ret.push_back(strip(aux.substr(0, pos)));
		aux.erase(0, pos + delimiter.length());
	}
	ret.push_back(strip(aux));

	return ret;
}

bool parseBool(const std::string &value, const bool required, const bool default_,
               const char* error_msg,
			   std::function<bool(bool)> check_method )
{
	const char* true_str[] = {"y","yes","t","true","1", ""};
	const char* false_str[] = {"n","no","f","false","0", ""};
	bool set = (!required && value == "");
	bool ret = default_;

	if (!set) {
		for (const char** i = true_str; **i != '\0'; i++) {
			if (value == *i) {
				ret = true; set = true;
			}
		}
	}
	if (!set) {
		for (const char** i = false_str; **i != '\0'; i++) {
			if (value == *i) {
				ret = false; set = true;
			}
		}
	}

	if (!set)
		throw std::invalid_argument(error_msg);
	if (check_method != nullptr && !check_method(ret))
		throw std::invalid_argument(error_msg);

	return ret;
}

uint32_t parseUint32(const std::string &value, const bool required, const uint32_t default_,
               const char* error_msg,
			   std::function<bool(uint32_t)> check_method )
{
	if (required && value == "")
		throw std::invalid_argument(error_msg);
	uint32_t ret = default_;
	try {
		if (value != "")
			ret = std::stoul(value);
	} catch (std::exception& e) {
		throw std::invalid_argument(error_msg);
	}
	if (check_method != nullptr && !check_method(ret))
		throw std::invalid_argument(error_msg);
	return ret;
}

uint64_t parseUint64(const std::string &value, const bool required, const uint64_t default_,
               const char* error_msg,
			   std::function<bool(uint64_t)> check_method )
{
	if (required && value == "")
		throw std::invalid_argument(error_msg);
	uint64_t ret = default_;
	try {
		if (value != "")
			ret = std::stoull(value);
	} catch (std::exception& e) {
		throw std::invalid_argument(error_msg);
	}
	if (check_method != nullptr && !check_method(ret))
		throw std::invalid_argument(error_msg);
	return ret;
}

double parseDouble(const std::string &value, const bool required, const double default_,
               const char* error_msg,
			   std::function<bool(double)> check_method )
{
	if (required && value == "")
		throw std::invalid_argument(error_msg);
	double ret = default_;
	try {
		if (value != "")
			ret = std::stod(value);
	} catch (std::exception& e) {
		throw std::invalid_argument(error_msg);
	}
	if (check_method != nullptr && !check_method(ret))
		throw std::invalid_argument(error_msg);
	return ret;
}

std::string vsprintf(const char* format, va_list args) {
	std::unique_ptr<char[]> buffer;
	va_list args_copy;

	for (size_t bs=256; bs*=2; bs<=1024) {
		va_copy(args_copy, args);
		buffer.reset(new char[bs]);
		auto r = vsnprintf(buffer.get(), bs, format, args_copy);
		va_end(args_copy);
		if ((r >= 0) && (r < bs)) break;
	}

	return std::string(buffer.get());
}

std::string sprintf(const char* format, ...) {
	va_list args;
	va_start(args, format);
	std::string ret = vsprintf(format, args);
	va_end(args);
	return ret;
}

} // namespace alutils
