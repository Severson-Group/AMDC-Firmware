#ifdef APP_PCBTEST

#include "usr/pcbtest/cmd/cmd_test.h"
#include "drv/analog.h"
#include "drv/pwm.h"
#include "sys/commands.h"
#include "sys/debug.h"
#include "sys/defines.h"
#include "sys/util.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

static command_help_t cmd_help[] = {
    { "analog", "Print all analog voltages" },
};

void cmd_test_register(void)
{
    commands_cmd_init(&cmd_entry, "test", "Test commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_test);

    // Register the command with the system
    commands_cmd_register(&cmd_entry);
}

static inline bool STREQ(char *in1, char *in2)
{
    return (strcmp(in1, in2) == 0) ? true : false;
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

    return CMD_INVALID_ARGUMENTS;
}

#endif // APP_PCBTEST
