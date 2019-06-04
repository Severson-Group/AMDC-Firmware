#include "cmd_enc.h"
#include "../defines.h"
#include "../debug.h"
#include "../commands.h"
#include "../../drv/encoder.h"
#include <stdint.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(2)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"steps", "Read encoder steps from power-up"},
		{"pos", "Read encoder position"}
};

void cmd_enc_register(void)
{
	// Populate the command entry block
	commands_cmd_init(&cmd_entry,
			"enc", "Encoder commands",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_enc
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

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
