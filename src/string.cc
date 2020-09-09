// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/string.h"
#include "alutils/internal.h"
#include "alutils/print.h"

#include <regex>
#include <stdexcept>
#include <memory>
#include <regex>
#include <type_traits>

#include <typeinfo>

#include <stdarg.h>

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ ""

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

////////////////////////////////////////////////////////////////////////////////////
bool debug_parse = false;

static bool stobool(const std::string& value) {
	const std::vector<std::string> true_str  {"y","yes","t","true","1"};
	const std::vector<std::string> false_str {"n","no","f","false","0"};

	auto value_strip = strip(value);
	for (auto i : true_str) {
		if (value_strip == i)
			return true;
	}
	for (auto i : false_str) {
		if (value_strip == i)
			return false;
	}
	throw std::runtime_error(sprintf("failed to convert the string \"%s\" to boolean", value.c_str()));
}

template<typename T>
inline T stoT(const std::string& value) {
	if constexpr (std::is_same<T, uint32_t>()) {
		return std::stoul(value);
	} else if constexpr (std::is_same<T, uint64_t>()) {
		return std::stoull(value);
	} else if constexpr (std::is_same<T, double>()) {
		return std::stod(value);
	} else if constexpr (std::is_same<T, bool>()) {
		return stobool(value);
	}
}

static const char* get_parse_error(std::string& dest, const std::string& value, const char* error_msg, const std::string& type) {
	if (error_msg != nullptr)
		return error_msg;
	dest = sprintf("failed to convert the string \"%s\" to type %s", value.c_str(), type.c_str());
	return dest.c_str();
}

template<typename T>
T parse(const std::string &value,
        const bool required, const T default_, const char* error_msg,
        std::function<bool(T)> check_method)
{
	std::string error_aux;

	if (debug_parse)
		PRINT_DEBUG("value=\"%s\"", value.c_str());

	if (required && value == "")
		throw std::invalid_argument(get_parse_error(error_aux, value, error_msg, typeid(T).name()));

	T ret = default_;
	try {
		if (value != "")
			ret = stoT<T>(value);
	} catch (std::exception& e) {
		throw std::invalid_argument(get_parse_error(error_aux, value, error_msg, typeid(T).name()));
	}

	if (check_method != nullptr && !check_method(ret))
		throw std::invalid_argument(get_parse_error(error_aux, value, error_msg, typeid(T).name()));
	return ret;
}

#define DECLARE_PARSER(TYPE)                                                  \
    template TYPE parse<TYPE>(const std::string &value, const bool required,  \
                              const TYPE default_, const char* error_msg,     \
                              std::function<bool(TYPE)> check_method)

DECLARE_PARSER(bool);
DECLARE_PARSER(uint32_t);
DECLARE_PARSER(uint64_t);
DECLARE_PARSER(double);


////////////////////////////////////////////////////////////////////////////////////
bool debug_parseSuffix = false;

template <typename T>
inline T parseSuffix(const std::string& value, const std::map<std::string, T>& suffixes) {
	std::cmatch cm;
	std::string value_strip = strip(value);

	std::regex_search(value_strip.c_str(), cm, std::regex("^(-{0,1}[0-9]+\\.{0,1}[0-9]*)\\s*(.*)$"));
	if (debug_parseSuffix) {
		PRINT_DEBUG("value='%s', cm[2]='%s', cm[2]='%s'", value.c_str(), cm.str(1).c_str(), cm.str(2).c_str());
	}

	T val;
	try {
		val = stoT<T>(cm.str(1));
	} catch (const std::exception& e) {
		throw std::runtime_error(sprintf("failed to convert value \"%s\" from the string \"%s\": %s", cm.str(1).c_str(), value_strip.c_str(), e.what()).c_str());
	};

	auto suf = strip(cm.str(2));
	if (suf != "") {
		for (auto i : suffixes) {
			if (suf == i.first)
				return val * i.second;
		}
		throw std::runtime_error(sprintf("invalid suffix \"%s\" in the string \"%s\"", suf.c_str(), value_strip.c_str()));
	}
	return val;
}

#define DECLARE_PARSER_SUFFIX(NAME, TYPE)                                               \
    TYPE NAME(const std::string& value, const std::map<std::string, TYPE>& suffixes) {  \
        return parseSuffix<TYPE>(value, suffixes);                                      \
    }

DECLARE_PARSER_SUFFIX(parseUint32Suffix, uint32_t);
DECLARE_PARSER_SUFFIX(parseUint64Suffix, uint64_t);
DECLARE_PARSER_SUFFIX(parseDoubleSuffix, double);


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
