// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace alutils {

struct CmdBase {
	std::string name;
	virtual ~CmdBase();
	virtual void processValue(const std::string& value);
};

struct CmdUint32 : public CmdBase {
	typedef std::function<bool(uint32_t)> checker_t;
	typedef std::function<void(uint32_t)> handler_t;

	uint32_t* address = nullptr;
	checker_t checker = nullptr;
	handler_t handler = nullptr;
	virtual ~CmdUint32();
	void processValue(const std::string& value) override;
};

class Commands {
	std::vector<CmdBase*> cmd_list;

	public:
	~Commands();

	void interpret(const std::string& str);

	void registerUint32Address(const std::string& name, uint32_t* address=nullptr, CmdUint32::checker_t checker=nullptr, CmdUint32::handler_t handler=nullptr);
};

} // namespace alutils
