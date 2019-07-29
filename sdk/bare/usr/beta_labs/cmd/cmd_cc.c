#ifdef APP_BETA_LABS

#include "cmd_cc.h"
#include "../../../sys/defines.h"
#include "../../../sys/commands.h"
#include "../task_cc.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES (7)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
    {"init", "Start current controller"},
    {"deinit", "Stop current controller"},
    {"reset", "Reset current controller"},
    {"bw <mFreq>", "Set controller bandwidth"},
    {"offset <enc_pulses>", "Set DQ frame offset"},
    {"theta_e_src <enc|est>", "Set theta_e source between encoder and estimation"},
    {"omega_e_src <enc|est>", "Set omega_e source between encoder and estimation"}
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
int cmd_cc(int argc, char **argv)
{
    // Handle 'init' sub-command
    if (strcmp("init", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 2) return INVALID_ARGUMENTS;

        // Make sure cc task was not already inited
        if (task_cc_is_inited()) return FAILURE;

        task_cc_init();
        return SUCCESS;
    }

    // Handle 'deinit' sub-command
    if (strcmp("deinit", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 2) return INVALID_ARGUMENTS;

        // Make sure cc task was already inited
        if (!task_cc_is_inited()) return FAILURE;

        task_cc_deinit();
        task_cc_clear();
        return SUCCESS;
    }

    // Handle 'reset' sub-command
    if (argc == 2 && strcmp("reset", argv[1]) == 0) {
        task_cc_reset();
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

    // Handle 'theta_e_src' sub-command
    if (argc == 3 && strcmp("theta_e_src", argv[1]) == 0) {
        uint8_t use_encoder = 1;
        if (strcmp("enc", argv[2]) == 0) {
            use_encoder = 1;
        } else if (strcmp("est", argv[2]) == 0) {
            use_encoder = 0;
        } else {
            return INVALID_ARGUMENTS;
        }

        task_cc_set_theta_e_src(use_encoder);

        return SUCCESS;
    }

    // Handle 'omega_e_src' sub-command
    if (argc == 3 && strcmp("omega_e_src", argv[1]) == 0) {
        uint8_t use_encoder = 1;
        if (strcmp("enc", argv[2]) == 0) {
            use_encoder = 1;
        } else if (strcmp("est", argv[2]) == 0) {
            use_encoder = 0;
        } else {
            return INVALID_ARGUMENTS;
        }

        task_cc_set_omega_e_src(use_encoder);

        return SUCCESS;
    }

    return INVALID_ARGUMENTS;
}

#endif // APP_BETA_LABS
