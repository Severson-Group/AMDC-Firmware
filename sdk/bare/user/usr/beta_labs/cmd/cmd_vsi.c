#ifdef APP_BETA_LABS

#include "usr/beta_labs/cmd/cmd_vsi.h"
#include "usr/beta_labs/task_vsi.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include <string.h>
#include <stdlib.h>


static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES	(4)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
    {"init", "Start VSI task"},
    {"deinit", "Stop VSI task"},
    {"legs <phAIdx> <phBIdx> <phCIdx>", "Set the output leg indices"},
    {"set <vPercent> <freq> [mRamptime]", "Set operating point (optional transition ramp time)"}
};

void cmd_vsi_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry,
            "vsi", "Voltage source inverter commands",
            cmd_help, NUM_HELP_ENTRIES,
            cmd_vsi
    );

    // Register the command
    commands_cmd_register(&cmd_entry);
}

int cmd_vsi(int argc, char **argv) {
    // Handle 'init' sub-command
    if (argc == 2 && strcmp("init", argv[1]) == 0) {
        // Make sure task was not already inited
        if (task_vsi_is_inited()) return FAILURE;

        task_vsi_init();
        return SUCCESS;
    }

    // Handle 'deinit' sub-command
    if (argc == 2 && strcmp("deinit", argv[1]) == 0) {
        // Make sure task was not already inited
        if (!task_vsi_is_inited()) return FAILURE;

        task_vsi_deinit();
        return SUCCESS;
    }

    // Handle 'legs ...' sub-command
    if (argc == 5 && strcmp("legs", argv[1]) == 0) {
        // Parse out phAIdx
        // and clip to 0..23
        int phAIdx = atoi(argv[2]);
        if (phAIdx <  0) return INVALID_ARGUMENTS;
        if (phAIdx > 23) return INVALID_ARGUMENTS;

        // Parse out phBIdx
        // and clip to 0..23
        int phBIdx = atoi(argv[3]);
        if (phBIdx <  0) return INVALID_ARGUMENTS;
        if (phBIdx > 23) return INVALID_ARGUMENTS;

        // Parse out phCIdx
        // and clip to 0..23
        int phCIdx = atoi(argv[4]);
        if (phCIdx <  0) return INVALID_ARGUMENTS;
        if (phCIdx > 23) return INVALID_ARGUMENTS;

        task_vsi_set_legs(phAIdx, phBIdx, phCIdx);

        return SUCCESS;
    }

    // Handle 'set ...' sub-command
    if ((argc == 4 || argc == 5) && strcmp("set", argv[1]) == 0) {
        // Parse out vPercent
        // and clip to 0..100
        int vPercent = atoi(argv[2]);
        if (vPercent <   0) return INVALID_ARGUMENTS;
        if (vPercent > 100) return INVALID_ARGUMENTS;

        // Parse out freq
        // and clip to 0..10,000
        int freq = atoi(argv[3]);
        if (freq <     0) return INVALID_ARGUMENTS;
        if (freq > 10000) return INVALID_ARGUMENTS;

        // Parse out mRamptime (optional)
        // and clip to 0..60sec
        int mRamptime = 0;
        if (argc == 5) {
            mRamptime = atoi(argv[4]);
            if (mRamptime <     0) return INVALID_ARGUMENTS;
            if (mRamptime > 60000) return INVALID_ARGUMENTS;
        }

        task_vsi_set(vPercent, freq, (double) mRamptime / 1000.0);

        return SUCCESS;
    }

    return INVALID_ARGUMENTS;
}

#endif // APP_BETA_LABS
