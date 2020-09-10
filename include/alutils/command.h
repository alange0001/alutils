// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "alutils/process.h"

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////

struct CmdBase {
	std::string name;
	virtual ~CmdBase();
	virtual void test(const std::string& value); // test a command without set
	virtual void set(const std::string& value);  // set a command
};

////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct CmdTemplate : public CmdBase {
	typedef std::function<bool(T)> checker_t;
	typedef std::function<void(T)> handler_t;

	bool      required = true;
	T         default_ = (T)0;
	T*        address = nullptr;
	checker_t checker = nullptr;
	handler_t handler = nullptr;
	CmdTemplate(const std::string& name, bool required=true, T default_=(T)0, T* address=nullptr, checker_t checker=nullptr, handler_t handler=nullptr);
	virtual ~CmdTemplate();
	void test(const std::string& value) override;
	void set(const std::string& value) override;
};

typedef CmdTemplate<bool>     CmdBool;
typedef CmdTemplate<int32_t>  CmdInt32;
typedef CmdTemplate<int64_t>  CmdInt64;
typedef CmdTemplate<uint32_t> CmdUint32;
typedef CmdTemplate<uint64_t> CmdUint64;
typedef CmdTemplate<double>   CmdDouble;


////////////////////////////////////////////////////////////////////////////////////

struct ScriptCommand {
	uint64_t    time = 0;
	std::string command;

	ScriptCommand(const std::string& str);
};

////////////////////////////////////////////////////////////////////////////////////

class Commands {
	std::vector<CmdBase*> cmd_list;

	// script variables
	std::chrono::system_clock::time_point time_ini;
	std::vector<ScriptCommand>            parsed_script;
	std::unique_ptr<ThreadController>     script_thread;

	public:
	Commands();
	~Commands();

	void monitorScript(const std::string& script, const std::string& delimiter=";", bool reset_time=false);
	bool isScriptActive(bool throw_exception=true);

	void parseCommand(const std::string& str, bool set_value=true);
	void registerCmd( CmdBase* cmd );
};

} // namespace alutils
