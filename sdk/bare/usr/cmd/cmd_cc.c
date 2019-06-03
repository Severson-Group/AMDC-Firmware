#include "cmd_cc.h"
#include "../../sys/defines.h"
#include "../task_cc.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//
// Handles the 'cc' command
// and all sub-commands
//
int cmd_cc(char **argv, int argc)
{
	// Handle 'init' sub-command
	if (strcmp("init", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure mc task was not already inited
		if (task_cc_is_inited()) return FAILURE;

		task_cc_init();
		return SUCCESS;
	}

	// Handle 'deinit' sub-command
	if (strcmp("deinit", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		// Make sure mc task was already inited
		if (!task_cc_is_inited()) return FAILURE;

		task_cc_deinit();
		task_cc_set_Id_star(0.0);
		task_cc_set_Iq_star(0.0);
		return SUCCESS;
	}

	// Handle 'Iq*' sub-command
	if (strcmp("Iq*", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Pull out iq argument
		double Iq_star = (double) atoi(argv[2]);
		Iq_star /= 1000.0;

		task_cc_set_Iq_star(Iq_star);
		return SUCCESS;
	}

	// Handle 'Id*' sub-command
	if (strcmp("Id*", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Pull out iq argument
		double Id_star = (double) atoi(argv[2]);
		Id_star /= 1000.0;

		task_cc_set_Id_star(Id_star);
		return SUCCESS;
	}

	// Handle 'offset' sub-command
	if (strcmp("offset", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Pull out offset argument
		int32_t offset = atoi(argv[2]);

		task_cc_set_dq_offset(offset);
		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}
