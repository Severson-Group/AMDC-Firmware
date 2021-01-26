#ifdef APP_PCBTEST

#include "usr/pcbtest/cmd/cmd_test.h"
#include "drv/analog.h"
#include "drv/pwm.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/util.h"
#include "usr/pcbtest/sm_test.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

static command_help_t cmd_help[] = {
    { "analog", "Print all analog voltages" },
    { "auto <quiet|loud> <num_passes> <#[T|B]> <VDRIVE>",
      "Perform automated test of specified power stack port (e.g. 1T is top-left)" },
};

void cmd_test_register(void)
{
    commands_cmd_init(&cmd_entry, "test", "Test commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_test);

    // Register the command with the system
    commands_cmd_register(&cmd_entry);
}

int cmd_test(int argc, char **argv)
{
    if (argc == 2 && STREQ("analog", argv[1])) {
        // Print analog voltages
        for (int i = 0; i < ANALOG_NUM_CHANNELS; i++) {
            float voltage;
            analog_getf(i, &voltage);
            debug_printf("%d: %8.4f\r\n", i, voltage);
        }

        return CMD_SUCCESS;
    }

    if (argc == 6 && STREQ("auto", argv[1])) {
        // Parse verboseness mode
        bool quiet_mode;
        if (STREQ("quiet", argv[2])) {
            quiet_mode = true;
        } else if (STREQ("loud", argv[2])) {
            quiet_mode = false;
        } else {
            return CMD_INVALID_ARGUMENTS;
        }

        // Parse num passes
        int num_passes = atoi(argv[3]);
        if (num_passes <= 0 || num_passes >= 1e3) {
            return CMD_INVALID_ARGUMENTS;
        }

        if (strlen(argv[4]) == 2) {
            // Parse stack number
            int stack = argv[4][0] - '0';
            if (stack < 1 || stack > 4) {
                return CMD_INVALID_ARGUMENTS;
            }

            // Parse top or bottom (T or B)
            char tb = argv[4][1];
            bool valid = (tb == 'T') | (tb == 'B');
            if (!valid) {
                return CMD_INVALID_ARGUMENTS;
            }

            // Parse VDRIVE voltage
            double vdrive = strtod(argv[5], NULL);
            if (vdrive < 5.0 || vdrive > 18.0) {
                return CMD_INVALID_ARGUMENTS;
            }

            if (sm_test_start_auto_test(quiet_mode, num_passes, stack, tb, vdrive) != SUCCESS) {
                return CMD_FAILURE;
            }

            return CMD_SUCCESS;
        }
    }

    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_PCBTEST
