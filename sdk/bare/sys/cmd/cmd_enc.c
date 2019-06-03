#include "cmd_enc.h"
#include "../defines.h"
#include "../debug.h"
#include "../../drv/encoder.h"
#include <stdint.h>
#include <string.h>

//
// Handles the 'enc' command
// and all sub-commands
//
int cmd_enc(char **argv, int argc)
{
	char msg[128];

	// Handle 'steps' sub-command
	if (strcmp("steps", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		int32_t steps;
		encoder_get_steps(&steps);

		snprintf(msg, 128, "steps: %ld\r\n", steps);
		debug_print(msg);

		return SUCCESS;
	}

	// Handle 'pos' sub-command
	if (strcmp("pos", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 2) return INVALID_ARGUMENTS;

		uint32_t position;
		encoder_get_position(&position);

		snprintf(msg, 128, "pos: %ld\r\n", position);
		debug_print(msg);

		return SUCCESS;
	}

	return INVALID_ARGUMENTS;
}
