// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/command.h"
#include "alutils/string.h"
#include "alutils/internal.h"
#include "alutils/print.h"

#include <type_traits>

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "CmdBase::"

CmdBase::~CmdBase() {}
void CmdBase::test(const std::string& value){}
void CmdBase::set(const std::string& value){}

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "CmdTemplate::"

template <typename T>
CmdTemplate<T>::CmdTemplate(const std::string& name, bool required, const T default_, T* address, checker_t checker, handler_t handler):
	required(required), default_(default_), address(address), checker(checker), handler(handler)
{
	this->name = name;
	if (address == nullptr && handler == nullptr)
		throw std::runtime_error("either address or handler must be not null");
}

template <typename T>
CmdTemplate<T>::~CmdTemplate(){
	PRINT_DEBUG("destructor");
}

template <typename T>
void CmdTemplate<T>::test(const std::string& value) {
	PRINT_DEBUG("test command=\"%s\", value=\"%s\"", name.c_str(), value.c_str());
	parse<T>(value, required, default_, sprintf("test failed for the command \"%s\" value \"%s\"", name.c_str(), value.c_str()).c_str(), checker);
}

template <typename T>
void CmdTemplate<T>::set(const std::string& value) {
	PRINT_DEBUG("set command=\"%s\", value=\"%s\"", name.c_str(), value.c_str());
	T aux = parse<T>(value, required, default_, sprintf("invalid value for the command \"%s\": \"%s\"", name.c_str(), value.c_str()).c_str(), checker);
	if (address)
		*address = aux;
	if (handler)
		handler(aux);
}

template class CmdTemplate<bool>;
template class CmdTemplate<int32_t>;
template class CmdTemplate<int64_t>;
template class CmdTemplate<uint32_t>;
template class CmdTemplate<uint64_t>;
template class CmdTemplate<double>;
template class CmdTemplate<std::string>;

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "ScriptCommand::"

ScriptCommand::ScriptCommand(const std::string& str) {
	const std::map<std::string, uint64_t> time_suffixes { {"s",1}, {"m",60} };

	auto c2 = split_str(str, ":");
	if (c2.size() > 2)
		throw std::runtime_error(sprintf("invalid command format: \"%s\"", str.c_str()));

	if (c2.size() > 1) {
		try {
			time = parseUint64Suffix(c2[0], time_suffixes);
		} catch (const std::exception& e) {
			throw std::runtime_error(sprintf("failed to parse time \"%s\": %s", c2[0].c_str(), e.what()));
		}
		command = c2[1];
	} else {
		command = c2[0];
	}
}

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "Commands::"

Commands::Commands() {
	time_ini = std::chrono::system_clock::now();
}

Commands::~Commands() {
	PRINT_DEBUG("destructor begin");
	for (auto i : cmd_list) {
		delete i;
	}
	PRINT_DEBUG("destructor end");
}

void Commands::setScriptDelimiter(const std::string& delimiter) {
	script_delimiter = delimiter;
}

void Commands::monitorScript(const std::string& script, bool reset_time) {
	PRINT_DEBUG("script=\"%s\", delimiter=\"%s\"", script.c_str(), script_delimiter.c_str());
	if (script_thread.get() != nullptr && script_thread->isActive(false))
		throw std::runtime_error("monitorScript is already active");

	script_thread.reset(nullptr);
	auto commands = split_str(script, script_delimiter);
	parsed_script.clear();

	// test commands before initiating the thread
	PRINT_DEBUG("testing commands");
	for (auto c : commands) {
		if (strip(c) == "") continue;
		ScriptCommand c2(c);
		parseCommand(c2.command, false);
		parsed_script.push_back(c2);
	}

	if (reset_time)
		time_ini = std::chrono::system_clock::now();

	// launch monitor thread
	PRINT_DEBUG("launch monitor thread");
	script_thread.reset(new ThreadController( [this](ThreadController::stop_t stop) {
		auto time_ini = this->time_ini;

		uint64_t time_elapsed;

		for (auto c : this->parsed_script) {
			// whait until stop or time_command <= time_elapsed
			while (!stop()) {
				time_elapsed = std::chrono::duration_cast<std::chrono::seconds>(
						std::chrono::system_clock::now() - time_ini ).count();
				if (c.time <= time_elapsed) {
					PRINT_DEBUG("time_command=%s, time_elapsed=%s, executind command \"%s\"", std::to_string(c.time).c_str(), std::to_string(time_elapsed).c_str(), c.command.c_str());
					this->parseCommand(c.command);
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			}
		}
	} ) );
}

bool Commands::isScriptActive(bool throw_exception) {
	if (script_thread.get() == nullptr)
		return false;
	return script_thread->isActive(throw_exception);
}

void Commands::parseCommand(const std::string& str, bool set_value) {
	PRINT_DEBUG("str=\"%s\", set_value=%s", str.c_str(), std::to_string(set_value).c_str());
	auto key_val = split_str(str, "=");
	if (key_val.size() > 2)
		throw std::runtime_error(sprintf("invalid command format \"%s\"", str.c_str()));

	std::string key = key_val[0];
	std::string value = "";
	if (key_val.size() >= 2)
		value = key_val[1];

	PRINT_DEBUG("command=\"%s\", value=\"%s\"", key.c_str(), value.c_str());
	for (auto i : cmd_list) {
		if (i->name == key) {
			if (set_value) {
				i->set(value);
				if (afterChange != nullptr)
					afterChange(this, key, value);
			} else {
				i->test(value);
			}
			return;
		}
	}
	throw std::runtime_error(sprintf("invalid command \"%s\"", key.c_str()));
}

void Commands::registerCmd( CmdBase* cmd ) {
	cmd_list.push_back(cmd);
}

} // namespace alutils
