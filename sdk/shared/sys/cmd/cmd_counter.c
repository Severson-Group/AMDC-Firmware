#include "sys/cmd/cmd_counter.h"
#include "sys/commands.h"
#include "sys/defines.h"
#include "sys/util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static command_entry_t cmd_entry;

static command_help_t cmd_help[] = {
    { "inc", "Increment CNT reg" },
    { "dec", "Decrement CNT reg" },
    { "clr", "Clear CNT reg" },
    { "get", "Get CNT reg" },
};

void cmd_counter_register(void)
{
    // Populate the command entry block
    commands_cmd_init(
        &cmd_entry, "counter", "Counter reg related commands", cmd_help, ARRAY_SIZE(cmd_help), cmd_counter);

    // Register the command
    commands_cmd_register(&cmd_entry);
}

static int CNT = 0;

// Handles the 'counter' command and all sub-commands
int cmd_counter(int argc, char **argv)
{
    if (argc == 2 && STREQ("inc", argv[1])) {
        CNT++;
        return CMD_SUCCESS;
    }

    if (argc == 2 && STREQ("dec", argv[1])) {
        CNT--;
        return CMD_SUCCESS;
    }

    if (argc == 2 && STREQ("clr", argv[1])) {
        CNT = 0;
        return CMD_SUCCESS;
    }

    if (argc == 2 && STREQ("get", argv[1])) {
        cmd_resp_printf("CNT = %d\r\n", CNT);
        return CMD_SUCCESS;
    }

    return CMD_INVALID_ARGUMENTS;
}
