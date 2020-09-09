// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include <alutils/command.h>
#include <alutils/print.h>

#include <cassert>

using namespace alutils;

int main(int argc, char** argv) {
	printf("\n\n=====================\ncommand-test:\n");
	log_level = LOG_DEBUG;
	{
		Commands commands;

		uint32_t cmd1, cmd2;
		commands.registerUint32Address("cmd1", &cmd1);
		commands.registerUint32Address("cmd2", &cmd2);

		commands.parseCommand("cmd1=123");
		commands.parseCommand("cmd1 = 456");
		commands.parseCommand(" cmd1 = 789 ");
		try { commands.parseCommand(" cmd1 = abc "); } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		try { commands.parseCommand(" cmd1"); } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		try { commands.parseCommand(" cmd1 = ="); } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		printf("cmd1 = %s\n", std::to_string(cmd1).c_str());
		assert( cmd1 == 789 );

		commands.parseCommand("cmd2=332");
		try { commands.parseCommand(" cmd2 = abc "); } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert( cmd2 == 332 );

		try { commands.parseCommand(" cmdXX = abc "); } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }

		commands.monitorScript("1:cmd1=1;2s:cmd2=2;3m:cmd1=3");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		assert( cmd1 == 1 );
		assert( cmd2 == 2 );
	}
	printf("OK!!\n");
	return 0;
}
