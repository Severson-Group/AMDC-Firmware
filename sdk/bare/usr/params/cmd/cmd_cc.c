#ifdef APP_PARAMS

#include "cmd_cc.h"
#include "../../../sys/defines.h"
#include "../../../sys/commands.h"
#include "../task_cc.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(7)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
		{"init", "Start current controller"},
		{"deinit", "Stop current controller"},
		{"bw <mFreq>", "Set controller bandwidth"},
		{"offset <enc_pulses>", "Set DQ frame offset"},
		{"inj <Id*|Iq*|Vd*|Vq*> <add|set> const <mValue>", "Inject a constant into controller"},
		{"inj <Id*|Iq*|Vd*|Vq*> <add|set> noise <mGain>", "Inject noise into controller"},
		{"inj <Id*|Iq*|Vd*|Vq*> <add|set> chirp <mGain> <mFreqMin> <mFreqMax> <mPeriod>", "Inject chirp into controller"},
};

void cmd_cc_register(void)
{
	// Populate the command entry block
	commands_cmd_init(&cmd_entry,
			"cc", "Current controller commands",
			cmd_help, NUM_HELP_ENTRIES,
			cmd_cc
	);

	// Register the command
	commands_cmd_register(&cmd_entry);
}

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
		task_cc_clear();
		return SUCCESS;
	}

	// Handle 'bw' sub-command
	if (strcmp("bw", argv[1]) == 0) {
		// Check correct number of arguments
		if (argc != 3) return INVALID_ARGUMENTS;

		// Pull out mAmp argument
		// and saturate to 1 .. 500Hz
		double mBw = (double) atoi(argv[2]);
		if (mBw < 1000.0) return INVALID_ARGUMENTS;
		if (mBw > 500000.0) return INVALID_ARGUMENTS;

		task_cc_set_bw(mBw / 1000.0);
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

	// Handle 'inj' sub-command
	if (strcmp("inj", argv[1]) == 0) {

		// Make sure cmd has >= 4 args
		if (argc < 4) return INVALID_ARGUMENTS;

		// Pull out Id*/Iq*/Vd*/Vq* argument
		cc_inj_value_e cmd_value;
		cc_inj_axis_e cmd_axis;
		if (strcmp("Id*", argv[2]) == 0) {
			cmd_value = CURRENT;
			cmd_axis = D_AXIS;
		} else if (strcmp("Iq*", argv[2]) == 0) {
			cmd_value = CURRENT;
			cmd_axis = Q_AXIS;
		} else if (strcmp("Vd*", argv[2]) == 0) {
			cmd_value = VOLTAGE;
			cmd_axis = D_AXIS;
		} else if (strcmp("Vq*", argv[2]) == 0) {
			cmd_value = VOLTAGE;
			cmd_axis = Q_AXIS;
		} else {
			return INVALID_ARGUMENTS;
		}

		// Pull out op argument
		cc_inj_op_e cmd_op;
		if (strcmp("set", argv[3]) == 0) {
			cmd_op = SET;
		} else if (strcmp("add", argv[3]) == 0) {
			cmd_op = ADD;
		} else {
			return INVALID_ARGUMENTS;
		}

		// Handle 'const' cmd
		if (strcmp("const", argv[4]) == 0) {
			// Check correct number of arguments
			if (argc != 6) return INVALID_ARGUMENTS;

			// Pull out mAmp argument
			// and saturate to -5A .. 5A
			double mGain = (double) atoi(argv[5]);
			if (mGain < -5000.0) return INVALID_ARGUMENTS;
			if (mGain >  5000.0) return INVALID_ARGUMENTS;

			task_cc_cmd_const(cmd_value, cmd_axis, cmd_op, mGain / 1000.0);

			return SUCCESS;
		}

		// Handle 'noise' cmd
		if (strcmp("noise", argv[4]) == 0) {
			// Check correct number of arguments
			if (argc != 6) return INVALID_ARGUMENTS;

			// Pull out mGain argument
			// and saturate to 0 .. 5A
			double mGain = (double) atoi(argv[5]);
			if (mGain < 0.0) return INVALID_ARGUMENTS;
			if (mGain > 5000.0) return INVALID_ARGUMENTS;

			task_cc_cmd_noise(cmd_value, cmd_axis, cmd_op, mGain / 1000.0);

			return SUCCESS;
		}

		// Handle 'chirp' cmd
		if (strcmp("chirp", argv[4]) == 0) {
			// Check correct number of arguments
			if (argc != 9) return INVALID_ARGUMENTS;

			// Pull out mGain argument
			// and saturate to 0 .. 5A
			double mGain = (double) atoi(argv[5]);
			if (mGain < 0.0) return INVALID_ARGUMENTS;
			if (mGain > 5000.0) return INVALID_ARGUMENTS;

			// Pull out mFreqMin argument
			// and saturate to 1 .. 1000Hz
			double mFreqMin = (double) atoi(argv[6]);
			if (mFreqMin < 1000.0) return INVALID_ARGUMENTS;
			if (mFreqMin > 1000000.0) return INVALID_ARGUMENTS;

			// Pull out mFreqMax argument
			// and saturate to 1 .. 1000Hz
			double mFreqMax = (double) atoi(argv[7]);
			if (mFreqMax < 1000.0) return INVALID_ARGUMENTS;
			if (mFreqMax > 1000000.0) return INVALID_ARGUMENTS;

			// Pull out mPeriod argument
			// and saturate to 1 .. 10 sec
			double mPeriod = (double) atoi(argv[8]);
			if (mPeriod < 1000.0) return INVALID_ARGUMENTS;
			if (mPeriod > 10000.0) return INVALID_ARGUMENTS;

			task_cc_cmd_chirp(
					cmd_value,
					cmd_axis,
					cmd_op,
					mGain / 1000.0,
					mFreqMin / 1000.0,
					mFreqMax / 1000.0,
					mPeriod / 1000.0
					);


			return SUCCESS;
		}
	}

	return INVALID_ARGUMENTS;
}

#endif // APP_PARAMS
