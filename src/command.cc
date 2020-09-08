// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/command.h"
#include "alutils/string.h"
#include "alutils/internal.h"
#include "alutils/print.h"

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "CmdBase::"

CmdBase::~CmdBase() {}
void CmdBase::processValue(const std::string& value){}

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "CmdUint32::"

CmdUint32::~CmdUint32(){
	PRINT_DEBUG("destructor");
}

void CmdUint32::processValue(const std::string& value) {
	uint32_t aux = parseUint32(value, true, 0, sprintf("invalid value for the command %s: %s", name.c_str(), value.c_str()).c_str(), checker);
	if (address)
		*address = aux;
	if (handler)
		handler(aux);
}

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "Commands::"

Commands::~Commands() {
	PRINT_DEBUG("destructor begin");
	for (auto i : cmd_list) {
		delete i;
	}
	PRINT_DEBUG("destructor end");
}

void Commands::interpret(const std::string& str) {
	PRINT_DEBUG("str=\"%s\"", str.c_str());
	auto key_val = split_str(str, "=");
	if (key_val.size() != 2)
		throw std::runtime_error(sprintf("invalid command format \"%s\"", str.c_str()));

	PRINT_DEBUG("command=\"%s\", value=\"%s\"", key_val[0].c_str(), key_val[1].c_str());
	for (auto i : cmd_list) {
		if (i->name == key_val[0]) {
			i->processValue(key_val[1]);
			return;
		}
	}
	throw std::runtime_error(sprintf("invalid command \"%s\"", key_val[0]));
}

void Commands::registerUint32Address(const std::string& name, uint32_t* address, CmdUint32::checker_t checker, CmdUint32::handler_t handler) {
	if (address == nullptr && handler == nullptr)
		throw std::runtime_error("either address or handler must be not null");
	CmdUint32 *cmd = new CmdUint32();
	cmd->name = name;
	cmd->address = address;
	cmd->checker = checker;
	cmd->handler = handler;
	cmd_list.push_back(cmd);
}

} // namespace alutils
