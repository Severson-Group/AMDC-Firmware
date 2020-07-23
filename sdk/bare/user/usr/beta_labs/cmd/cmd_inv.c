#ifdef APP_BETA_LABS

#include "usr/beta_labs/cmd/cmd_inv.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "usr/beta_labs/inverter.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

#define NUM_HELP_ENTRIES (2)
static command_help_t cmd_help[NUM_HELP_ENTRIES] = {
    { "dtc <uDcomp> <mCurrLimit>", "Set deadtime compensation" },
    { "Vdc <mVdc>", "Set Vdc" },
};

void cmd_inv_register(void)
{
    // Populate the command entry block
    commands_cmd_init(&cmd_entry, "inv", "Inverter commands", cmd_help, NUM_HELP_ENTRIES, cmd_inv);

    // Register the command
    commands_cmd_register(&cmd_entry);
}

//
// Handles the 'inv' command
// and all sub-commands
//
int cmd_inv(int argc, char **argv)
{
    // Handle 'dtc' sub-command
    if (strcmp("dtc", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 4)
            return CMD_INVALID_ARGUMENTS;

        // Pull out uDcomp argument
        // and saturate to 0 .. 0.2
        double uDcomp = (double) atoi(argv[2]);
        if (uDcomp > 200000.0)
            return CMD_INVALID_ARGUMENTS;
        if (uDcomp < 0.0)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mCurrLimit argument
        // and saturate to 0 ... 4A
        double mCurrLimit = (double) atoi(argv[3]);
        if (mCurrLimit > 4000.0)
            return CMD_INVALID_ARGUMENTS;
        if (mCurrLimit < 0.0)
            return CMD_INVALID_ARGUMENTS;

        inverter_set_dtc(uDcomp / 1000000.0, mCurrLimit / 1000.0);
        return CMD_SUCCESS;
    }

    // Handle 'Vdc' sub-command
    if (strcmp("Vdc", argv[1]) == 0) {
        // Check correct number of arguments
        if (argc != 3)
            return CMD_INVALID_ARGUMENTS;

        // Pull out mVdc argument
        // and saturate to 0 .. 100V
        double mVdc = (double) atoi(argv[2]);
        if (mVdc > 100000.0)
            return CMD_INVALID_ARGUMENTS;
        if (mVdc < 0.0)
            return CMD_INVALID_ARGUMENTS;

        inverter_set_Vdc(mVdc / 1000.0);
        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_BETA_LABS
