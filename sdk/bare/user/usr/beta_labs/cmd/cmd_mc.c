#ifdef APP_BETA_LABS

#include "usr/beta_labs/cmd/cmd_mc.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "usr/beta_labs/task_mc.h"
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES (5)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
    { "init", "Start motion controller" },
    { "deinit", "Stop motion controller" },
    { "rpm <rpms>", "Set commanded speed" },
    { "cff <on|off>", "Enable / disable command feed-forward" },
    { "omega_m_src <enc|est>", "Set omega_m source between encoder and estimation" },
};

void cmd_mc_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry, "mc", "Motion controller commands", cmd_help, NUM_HELP_ENTRIES, cmd_mc);

    // Register the command
    commands_cmd_register(&cmd_entry);
}

int cmd_mc(int argc, char **argv)
{
    // Handle 'init' sub-command
    if (argc == 2 && strcmp("init", argv[1]) == 0) {
        // Make sure mc task was not already inited
        if (task_mc_is_inited())
            return CMD_FAILURE;

        task_mc_init();
        return CMD_SUCCESS;
    }

    // Handle 'deinit' sub-command
    if (argc == 2 && strcmp("deinit", argv[1]) == 0) {
        // Make sure mc task was already inited
        if (!task_mc_is_inited())
            return CMD_FAILURE;

        task_mc_deinit();
        return CMD_SUCCESS;
    }

    // Handle 'rpm ...' sub-command
    if (argc == 3 && strcmp("rpm", argv[1]) == 0) {
        // Pull out rpms argument
        // and saturate to -10,000 .. 10,000 RPMs
        double rpms = (double) atoi(argv[2]);
        if (rpms < -10000.0)
            return CMD_INVALID_ARGUMENTS;
        if (rpms > 10000.0)
            return CMD_INVALID_ARGUMENTS;

        task_mc_set_omega_m_star(PI2 * rpms / 60.0);

        return CMD_SUCCESS;
    }

    // Handle 'cff ...' sub-command
    if (argc == 3 && strcmp("omega_m_src", argv[1]) == 0) {
        // Pull out est/enc argument
        uint8_t use_encoder;
        if (strcmp("enc", argv[2]) == 0) {
            use_encoder = 1;
        } else if (strcmp("est", argv[2]) == 0) {
            use_encoder = 0;
        } else {
            return CMD_INVALID_ARGUMENTS;
        }

        task_mc_set_omega_m_src(use_encoder);

        return CMD_SUCCESS;
    }

    // Handle 'cff ...' sub-command
    if (argc == 3 && strcmp("cff", argv[1]) == 0) {
        // Pull out onoff argument
        uint8_t enabled;
        if (strcmp("on", argv[2]) == 0) {
            // On
            enabled = 1;
        } else if (strcmp("off", argv[2]) == 0) {
            enabled = 0;
        } else {
            return CMD_INVALID_ARGUMENTS;
        }

        task_mc_set_cff_enabled(enabled);

        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_BETA_LABS
