
#ifdef APP_VSI

#include "usr/vsiApp/cmd/cmd_vsiApp.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/util.h"
#include <usr/vsiApp/task_vsiApp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Stores command entry for command system module
static command_entry_t cmd_entry;

// Defines help content displayed for this command
// when user types "help" at command prompt
static command_help_t cmd_help[] = {
    { "init", "Start task" },
    { "deinit", "Stop task" },
	{ "amplitude <value>", "set the analog wave to a specific amplitude" },
	{ "frequency <value>", "set the analog wave to a specific frequency" },
	{ "stats print", "Print stats to screen" },
	{ "stats reset", "Reset the task timing stats" }
};

void cmd_vsiApp_register(void)
{
    // Populate the command entry block
    //
    // Here is where you define the base command string: "blink"
    // and what function is called to handle command
    commands_cmd_init(&cmd_entry, "vsi", "VSI application commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_vsiApp);

    // Register the command with the system
    commands_cmd_register(&cmd_entry);
}

int cmd_vsiApp(int argc, char **argv)
{
    if (argc == 3 && strcmp("amplitude", argv[1]) == 0) {
    	double argDoub;
    	sscanf(argv[2], "%lf", &argDoub);
    	if (task_vsiApp_amplitude(argDoub) != SUCCESS) {
			return CMD_FAILURE;
		}
		return CMD_SUCCESS;
    }
    if (argc == 3 && strcmp("frequency", argv[1]) == 0) {
		double argDoub;
		sscanf(argv[2], "%lf", &argDoub);
		if (task_vsiApp_frequency(argDoub) != SUCCESS) {
			return CMD_FAILURE;
		}
		return CMD_SUCCESS;
	}

    if (argc == 2 && strcmp("init", argv[1]) == 0) {
        if (task_vsiApp_init() != SUCCESS) {
            return CMD_FAILURE;
        }
        return CMD_SUCCESS;
    }

    if (argc == 2 && strcmp("deinit", argv[1]) == 0) {
        if (task_vsiApp_deinit() != SUCCESS) {
            return CMD_FAILURE;
        }
        return CMD_SUCCESS;
    }
    if (argc == 3 && strcmp("stats", argv[1]) == 0) {
    	if (strcmp("print", argv[2]) == 0) {
    		task_vsiApp_stats_print();
    		return CMD_SUCCESS_QUIET;
    	}
    	if (strcmp("reset", argv[2]) == 0) {
    		task_vsiApp_stats_reset();
    		return CMD_SUCCESS;
    	}
    }
    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_VSI
