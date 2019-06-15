#ifdef APP_DEADTIME_COMP

#include "cmd_inv.h"
#include "../inverter.h"
#include "../../../sys/defines.h"
#include "../../../sys/commands.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(2)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"dtc <mDcomp> <mCurrLimit>", "Set deadtime compensation"},
		{"Vdc <mVdc>", "Set Vdc"}
};

void cmd_inv_register(void)
{
	// Populate the command entry block
	commands_cmd_init(&cmd_entry,
			"inv", "Inverter commands",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_inv
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

//
// Handles the 'inv' command
// and all sub-commands
//
int cmd_inv(char **argv, int argc)
{
	// Handle 'dtc' sub-command
	if (strcmp("dtc", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 4) return INVALID_ARGUMENTS;

		// Pull out mDcomp argument
		// and saturate to 0 .. 0.2
		double mDcomp = (double) atoi(argv[2]);
		if (mDcomp > 200.0) return INVALID_ARGUMENTS;
		if (mDcomp < 0.0) return INVALID_ARGUMENTS;

		// Pull out mCurrLimit argument
		// and saturate to 0 ... 2A
		double mCurrLimit = (double) atoi(argv[3]);
		if (mCurrLimit > 2000.0) return INVALID_ARGUMENTS;
		if (mCurrLimit < 0.0) return INVALID_ARGUMENTS;

		inverter_set_dtc(mDcomp / 1000.0, mCurrLimit / 1000.0);
		return SUCCESS;
	}

	// Handle 'Vdc' sub-command
	if (strcmp("Vdc", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Pull out mVdc argument
		// and saturate to 0 .. 100V
		double mVdc = (double) atoi(argv[2]);
		if (mVdc > 100000.0) return INVALID_ARGUMENTS;
		if (mVdc < 0.0) return INVALID_ARGUMENTS;

		inverter_set_Vdc(mVdc / 1000.0);
		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}

#endif // APP_DEADTIME_COMP
