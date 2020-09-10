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
		bool fail = false;
		Commands commands;

		uint32_t cmd1, cmd2;
		uint64_t cmd3;
		bool cmd4;
		commands.registerCmd( new CmdUint32("cmd1", true, 0, &cmd1) );
		commands.registerCmd( new CmdUint32("cmd2", true, 0, &cmd2) );
		commands.registerCmd( new CmdUint64("cmd3", true, 0, &cmd3) );
		commands.registerCmd( new CmdBool("cmd4", false, true, &cmd4) );

		commands.parseCommand("cmd1=123");
		assert( cmd1 == 123 );
		commands.parseCommand("cmd1 = 456");
		assert( cmd1 == 456 );
		commands.parseCommand(" cmd1 = 789 ");
		assert( cmd1 == 789 );

		try { commands.parseCommand(" cmd1 = abc "); fail=true; } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert(!fail);
		try { commands.parseCommand(" cmd1"); fail=true; } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert(!fail);
		try { commands.parseCommand(" cmd1 = ="); fail=true; } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert(!fail);

		assert( cmd1 == 789 );

		commands.parseCommand("cmd2=332");
		try { commands.parseCommand(" cmd2 = 2abc "); } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert(!fail);
		try { commands.parseCommand(" cmd2 = -2 "); fail=true; } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert(!fail);
		assert( cmd2 == 332 );

		try { commands.parseCommand(" cmdXX = abc "); fail=true; } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert(!fail);


		commands.parseCommand("cmd4=false");
		assert( cmd4 == false );
		commands.parseCommand("cmd4");
		assert( cmd4 == true );
		commands.parseCommand("cmd4=0");
		assert( cmd4 == false );
		try { commands.parseCommand(" cmd4 = abc "); fail=true; } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert(!fail);

		try { commands.monitorScript("1t::cmd1=1; cmd3=22; 2s:cmd2=2; 3m:cmd1=3"); fail=true; } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert(!fail);
		try { commands.monitorScript("1t:cmd1=1; cmd3=22; 2s:cmd2=2; 3m:cmd1=3"); fail=true; } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert(!fail);

		commands.monitorScript("1:cmd1=1; cmd3=22; 2s:cmd2=2");
		try { commands.monitorScript("1:cmd1=1; cmd3=22; 2s:cmd2=2; 3m:cmd1=3"); fail=true; } catch (std::exception &e) { printf("Expected exception: %s\n", e.what()); }
		assert(!fail);
		std::this_thread::sleep_for(std::chrono::seconds(3));
		assert( cmd1 == 1 );
		assert( cmd2 == 2 );
		assert( cmd3 == 22 );

		cmd4=false;
		commands.monitorScript("cmd1=111; 1:cmd3=2; cmd2=222; cmd3=333; cmd4; 3m:cmd1=2");
		std::this_thread::sleep_for(std::chrono::seconds(2));
		assert ( commands.isScriptActive() );
		assert( cmd1 == 111 );
		assert( cmd2 == 222 );
		assert( cmd3 == 333 );
		assert( cmd4 );

	}
	printf("OK!!\n");
	return 0;
}
