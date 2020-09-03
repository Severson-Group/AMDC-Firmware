#ifdef APP_DAC

#include "usr/dac/cmd/cmd_dac.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/util.h"
#include "usr/dac/task_dac.h"
#include <stdlib.h>
#include <string.h>

// Stores command entry for command system module
static command_entry_t cmd_entry;

// Defines help content displayed for this command
// when user types "help" at command prompt
static command_help_t cmd_help[] = {
    { "init", "Start task" },
    { "deinit", "Stop task" },
	{ "freq <frequency>", "Set Frequency"},
	{ "ch <channel> <voltage>", "Set channel to voltage"},
	{ "reg <register> <value(in hex)>", "Write value to register"},
	{ "trigger", "Trigger synchronous channels"},
	{ "broadcast <voltage>", "Broadcast voltage"},
};

void cmd_dac_register(void)
{
    // Populate the command entry block
    //
    // Here is where you define the base command string: "blink"
    // and what function is called to handle command
    commands_cmd_init(&cmd_entry, "dac", "DAC application commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_dac);

    // Register the command with the system
    commands_cmd_register(&cmd_entry);
}

int cmd_dac(int argc, char **argv)
{
    // This function is called every time the user types "blink"
    // followed by a space and any number of characters.
    //
    // Example user input: blink foo 123
    //
    // It is up to the application developer to handle this
    // incoming command in a manner that reflects this command's
    // help message, as to not confuse the user.
    //
    // The arguments passed into this function (argc & argv)
    // follow standard C convention for main() programs called
    // via command line interface (CLI).
    //
    // 'argc' is the number of CLI arguments, including the base command
    // For above example: argc = 3
    //
    // 'argv' is an array of char strings which contain the CLI arguments
    // For above example:
    // - argv[0] => "blink"
    // - argv[1] => "foo"
    // - argv[2] => "123"
    //
    // NOTE: The system constrains user CLI input to ensure responsive
    // behavior for arbitrary commands. This involves limiting individual
    // command argument length as well as the total number of arguments
    // (this is defined in sys/commands.c)

    // Handle 'hello' sub-command
    //
    // First, verify correct number of arguments (2)
    // Second, verify second argument is "hello"
    if (argc == 2 && strcmp("init", argv[1]) == 0) {
        if (task_dac_init() != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    if (argc == 2 && strcmp("deinit", argv[1]) == 0) {
        if (task_dac_deinit() != SUCCESS) {
            return CMD_FAILURE;
        }

        return CMD_SUCCESS;
    }

    if (argc == 3 && strcmp("freq", argv[1]) == 0) {
    	double frequency;
    	sscanf(argv[2],"%lf",&frequency);
    	task_dac_frequency(frequency);
    	return CMD_SUCCESS;
    }

    if (argc == 3 && strcmp("broadcast", argv[1]) == 0) {
		double voltage;
		sscanf(argv[2],"%lf",&voltage);
		task_dac_broadcast(voltage);
		return CMD_SUCCESS;
    }

    if (argc == 2 && strcmp("trigger", argv[1]) == 0) {
		task_dac_trigger();
		return CMD_SUCCESS;
    }

    if (argc == 4 && strcmp("ch", argv[1]) == 0) {
    	int ch;
    	sscanf(argv[2], "%i", &ch);

    	double voltage;
    	sscanf(argv[3],"%lf",&voltage);

    	task_dac_set_voltage((uint8_t)ch, voltage);

    	return CMD_SUCCESS;
    }

    if (argc == 4 && strcmp("reg", argv[1]) == 0) {
		int reg;
		sscanf(argv[2], "%i", &reg);

		uint32_t value;
		sscanf(argv[3],"%lX",&value);

		task_dac_set_reg((uint8_t)reg, value);

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

#endif // APP_BLINK
