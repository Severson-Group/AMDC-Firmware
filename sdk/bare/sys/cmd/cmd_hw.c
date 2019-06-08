#include "cmd_hw.h"
#include "../defines.h"
#include "../commands.h"
#include "../debug.h"
#include "../../drv/pwm.h"
#include "../../drv/encoder.h"
#include "../../drv/analog.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(5)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"pwm sw <freq_switching> <deadtime_ns>", "Set the PWM switching characteristics"},
		{"pwm duty <pwm_idx> <percent>", "Set a duty ratio"},
		{"anlg read <chnl_idx>", "Read voltage on ADC channel"},
		{"enc steps", "Read encoder steps from power-up"},
		{"enc pos", "Read encoder position"}
};

void cmd_hw_register(void)
{
	// Populate the command entry block
	commands_cmd_init(&cmd_entry,
			"hw", "Hardware related commands",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_hw
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

//
// Handles the 'hw' command
// and all sub-commands
//
int cmd_hw(char **argv, int argc)
{
	char msg[128];

	// Handle 'pwm' sub-command
	if (strcmp("pwm", argv[1]) == 0) {
		if (strcmp("sw", argv[2]) == 0) {
			// Check correct number of arguments
			if (argc != 5) return INVALID_ARGUMENTS;

			// Parse out switching freq arg
			double fsw = (double) atoi(argv[3]);
			if (fsw > 1000000.0) return INVALID_ARGUMENTS;
			if (fsw < 2000.0) return INVALID_ARGUMENTS;

			// Parse out dead time arg
			int dt = atoi(argv[4]);
			if (dt > 5000) return INVALID_ARGUMENTS;
			if (dt < 25) return INVALID_ARGUMENTS;

			pwm_set_deadtime_ns(dt);
			pwm_set_switching_freq(fsw);

			return SUCCESS;
		}

		if (strcmp("duty", argv[2]) == 0) {
			// Check correct number of arguments
			if (argc != 5) return INVALID_ARGUMENTS;

			// Parse out switching pwm_idx arg
			int pwm_idx = atoi(argv[3]);
			if (pwm_idx > 23) return INVALID_ARGUMENTS;
			if (pwm_idx < 0) return INVALID_ARGUMENTS;

			// Parse out percent arg
			int percent = atoi(argv[4]);
			if (percent > 100) return INVALID_ARGUMENTS;
			if (percent < 0) return INVALID_ARGUMENTS;

			pwm_set_duty(pwm_idx, (double) percent / 100.0);

			return SUCCESS;
		}
	}

	// Handle 'anlg' sub-command
	if (strcmp("anlg", argv[1]) == 0) {
		if (strcmp("read", argv[2]) == 0) {
			// Check correct number of arguments
			if (argc != 4) return INVALID_ARGUMENTS;

			// Parse out switching pwm_idx arg
			int anlg_idx = atoi(argv[3]);
			if (anlg_idx > 15) return INVALID_ARGUMENTS;
			if (anlg_idx < 0) return INVALID_ARGUMENTS;

			float value;
			analog_getf(anlg_idx + 1, &value);

			snprintf(msg, 128, "%fV\r\n", value);
			debug_print(msg);

			return SUCCESS;
		}
	}

	// Handle 'enc' sub-command
	if (strcmp("enc", argv[1]) == 0) {
		if (strcmp("steps", argv[2]) == 0) {
			// Check correct number of arguments
			if (argc != 3) return INVALID_ARGUMENTS;

			int32_t steps;
			encoder_get_steps(&steps);

			snprintf(msg, 128, "steps: %ld\r\n", steps);
			debug_print(msg);

			return SUCCESS;
		}

		if (strcmp("pos", argv[2]) == 0) {
			// Check correct number of arguments
			if (argc != 3) return INVALID_ARGUMENTS;

			uint32_t position;
			encoder_get_position(&position);

			snprintf(msg, 128, "pos: %ld\r\n", position);
			debug_print(msg);

			return SUCCESS;
		}
	}

	return INVALID_ARGUMENTS;
}
