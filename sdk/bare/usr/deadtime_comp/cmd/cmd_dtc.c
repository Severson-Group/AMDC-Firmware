#include "cmd_dtc.h"
#include "../task_dtc.h"
#include "../../../sys/defines.h"
#include "../../../sys/commands.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(3)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"init", "Start current controller"},
		{"deinit", "Stop current controller"},
		{"set <mA> <mHz>", "Command sinusoidal current"}
};

void cmd_dtc_register(void)
{
	// Populate the command entry block
	commands_cmd_init(&cmd_entry,
			"dtc", "Dead time compensation commands",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_dtc
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

//
// Handles the 'dtc' command
// and all sub-commands
//
int cmd_dtc(char **argv, int argc)
{
	// Handle 'init' sub-command
	if (strcmp("init", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure dtc task was not already inited
		if (task_dtc_is_inited()) return FAILURE;

		task_dtc_init();
		return SUCCESS;
	}

	// Handle 'deinit' sub-command
	if (strcmp("deinit", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure dtc task was already inited
		if (!task_dtc_is_inited()) return FAILURE;

		task_dtc_clear();
		task_dtc_deinit();
		return SUCCESS;
	}

	// Handle 'set' sub-command
	if (strcmp("set", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 4) return INVALID_ARGUMENTS;

		// Pull out mA argument
		// and saturate to +/- 5A
		double mA = (double) atoi(argv[2]);
		if (mA >  5000.0) return INVALID_ARGUMENTS;
		if (mA < -5000.0) return INVALID_ARGUMENTS;

		// Pull out mHz argument
		// and saturate to 0..10Hz
		double mHz = (double) atoi(argv[3]);
		if (mHz > 10000.0) return INVALID_ARGUMENTS;
		if (mHz < 0.0) return INVALID_ARGUMENTS;

		double Hz = mHz / 1000;
		double A = mA / 1000;

		task_dtc_set_I_star(A, Hz);
		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}
