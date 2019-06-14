#ifdef APP_PMSM_MC

#include "cmd_mc.h"
#include "../../../sys/defines.h"
#include "../../../sys/commands.h"
#include "../task_mc.h"
#include "../task_cc.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(3)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"init", "Start motion controller"},
		{"deinit", "Stop motion controller"},
		{"rpm <rpms>", "Command speed to motion controller"}
};

void cmd_mc_register(void)
{
	// Populate the command entry block
	commands_cmd_init(&cmd_entry,
			"mc", "Motion controller commands",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_mc
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

//
// Handles the 'mc' command
// and all sub-commands
//
int cmd_mc(char **argv, int argc)
{
	// Handle 'init' sub-command
	if (strcmp("init", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure mc task was not already inited
		if (task_mc_is_inited()) return FAILURE;

		task_mc_init();
		return SUCCESS;
	}

	// Handle 'deinit' sub-command
	if (strcmp("deinit", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure mc task was already inited
		if (!task_mc_is_inited()) return FAILURE;

		task_mc_deinit();
		task_mc_set_omega_star(0.0);
		task_cc_set_Id_star(0.0);
		task_cc_set_Iq_star(0.0);
		return SUCCESS;
	}

	// Handle 'rpm' sub-command
	if (strcmp("rpm", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Pull out rpm argument
		double rpms = (double) atoi(argv[2]);

		// Saturate commanded speed to +/- 600 RPM
		if (rpms >  2000.0) return INVALID_ARGUMENTS;
		if (rpms < -2000.0) return INVALID_ARGUMENTS;

		double rad_per_sec = rpms * PI / 30.0;

		task_mc_set_omega_star(rad_per_sec);
		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}

#endif // APP_PMSM_MC

