#include "cmd_mc.h"
#include "../../sys/defines.h"
#include "../task_mc.h"
#include "../task_cc.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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
		if (rpms >  600.0) rpms =  600.0;
		if (rpms < -600.0) rpms = -600.0;

		double rad_per_sec = rpms * PI / 30.0;

		task_mc_set_omega_star(rad_per_sec);
		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}
