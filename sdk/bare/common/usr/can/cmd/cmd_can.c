#ifdef APP_CAN

#include "usr/can/cmd/cmd_can.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/util.h"
#include "usr/can/task_can.h"
#include "xcanps.h"
#include <stdlib.h>
#include <string.h>

// Stores command entry for command system module
static command_entry_t cmd_entry;

// Defines help content displayed for this command
// when user types "help" at command prompt
static command_help_t cmd_help[] = {
    { "init", "Start task" },
    { "deinit", "Stop task" },
	{ "selftest", "Run test message in loopback mode"},
    { "send <number of bytes>", "Send a predefined message with specified number of bytes" },
	{ "print", "Prints latest message"},
	{ "setmode <mode>", "Set CAN mode {loopback, sleep, config, normal}"},
	{ "setbaud <baudrate>", "Set CAN baudrate (type 0 for default)"},
	{ "set btr <jump> <first time> <second time>", "Set CAN bit timing register (type 0s for default)"},
};

void cmd_can_register(void)
{
    // Populate the command entry block
    //
    // Here is where you define the base command string: "can"
    // and what function is called to handle command
	commands_cmd_init(&cmd_entry, "can", "CAN application commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_can);

    // Register the command with the system
    commands_cmd_register(&cmd_entry);
}


int cmd_can(int argc, char **argv)
{
    // This function is called every time the user types "can"
    // followed by a space and any number of characters.

    // Handle 'init' sub-command
    //
    // First, verify correct number of arguments (2)
    // Second, verify second argument is "init"
    if (argc == 2 && strcmp("init", argv[1]) == 0) {
        if (task_can_init() != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    // Handle 'deinit' sub-command
    //
    // First, verify correct number of arguments (2)
    // Second, verify second argument is "deinit"
    if (argc == 2 && strcmp("deinit", argv[1]) == 0) {
        if (task_can_deinit() != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    // Handle 'selftest' sub-command
	//
	// First, verify correct number of arguments (2)
	// Second, verify second argument is "test"
	if (argc == 2 && strcmp("selftest", argv[1]) == 0) {
		if (task_can_loopback_test() != SUCCESS) {
			return CMD_FAILURE;
		}
		return CMD_SUCCESS;
	}

	// Handle 'send' sub-command
	//
	// First, verify correct number of arguments (4)
	// Second, verify second argument is "send"
	if (argc == 3 && strcmp("send", argv[1]) == 0) {
		int num_bytes;
		sscanf(argv[2], "%i", &num_bytes);
		uint8_t packet[num_bytes];
		int i;
		for (i = 0; i < num_bytes; i++) {
			packet[i] = i;
		}
		if (task_can_sendmessage(packet, num_bytes) != SUCCESS)
			return CMD_FAILURE;
		return CMD_SUCCESS;

	}

	// Handle 'print' sub-command
	//
	// First, verify correct number of argument (2)
	// Second, verify second argument is "print"
	if (argc == 2 && strcmp("print", argv[1]) == 0) {
		if (task_can_print() != SUCCESS)
			return CMD_FAILURE;
		return CMD_SUCCESS;
	}

	// Handle 'setmode' sub-command
	//
	// First, verify correct number of arguments (3)
	// Second, verify second argument is "setmode"
	if (argc == 3 && strcmp("setmode", argv[1]) == 0) {
		if (strcmp("loopback", argv[2]) == 0) {
			if (task_can_setmode(XCANPS_MODE_LOOPBACK) != SUCCESS)
				return CMD_FAILURE;
		}
		else if (strcmp("normal", argv[2]) == 0) {
			if (task_can_setmode(XCANPS_MODE_NORMAL) != SUCCESS)
				return CMD_FAILURE;
		}
		else if (strcmp("config", argv[2]) == 0) {
			if (task_can_setmode(XCANPS_MODE_CONFIG) != SUCCESS)
				return CMD_FAILURE;
		}
		else if (strcmp("sleep", argv[2]) == 0) {
			if (task_can_setmode(XCANPS_MODE_SLEEP) != SUCCESS)
				return CMD_FAILURE;
		}
		return CMD_SUCCESS;
	}

	// Handle 'setbaud' sub-command
	//
	// First, verify correct number of arguments (3)
	// Second, verify second argument is "setbaud"
	if (argc == 3 && strcmp("setbaud", argv[1]) == 0) {
		 int baud;
		 sscanf(argv[2], "%i", &baud);
		 if (task_can_setbaud(baud) != SUCCESS) {
			 return CMD_FAILURE;
		 }
		 return CMD_SUCCESS;
	}

	// Handle 'set btr' sub-command
	//
	// First, verify correct number of argumnets (6)
	// Second, verify second argument is "set" and
	// Third, verify third argument is "btr"
	if (argc == 6 && strcmp("set", argv[1]) == 0 && strcmp("btr", argv[1]) == 0) {
		int jump;
		int first_time;
		int second_time;
		sscanf(argv[2], "%i", &jump);
		sscanf(argv[2], "%i", &first_time);
		sscanf(argv[2], "%i", &second_time);
		if (task_can_set_btr(jump, first_time, second_time) != SUCCESS) {
			return CMD_FAILURE;
		}
		return CMD_SUCCESS;
	}



    // At any point, if an error is detected in given input command,
    // simply return an error code (defined in sys/defines.h)
    //
    // The return statement below is used to catch all user input which
    // didn't match the if statements above. In general, to handle commands,
    // assume they are invalid. Only after checking if each argument is
    // valid should you trust them.
    //
    // Common error return values are: FAILURE, INVALID_ARGUMENTS
    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_CAN
